#include <iostream>
#include <optional>
#include <vector>
#include <windows.h>

class PEParser
{
public:
  [[nodiscard]] static PEParser* GetSingleton()
  {
    static PEParser singleton;
    return &singleton;
  }

  void OpenFile(const char* filename)
  {
    this->filename = filename;
    if (this->file != NULL) {
      fclose(this->file);
    }
    this->file = fopen(filename, "rb");

    if (this->file == NULL) {
      std::cerr << "unbale to open file\n";
      exit(2);
    }

    ReadByOffset(this->dos_header, 0);
    ReadByOffset(this->nt_headers, this->dos_header->e_lfanew);

    for (auto i = 0; i < this->nt_headers->FileHeader.NumberOfSections; i++) {
      IMAGE_SECTION_HEADER* section = new IMAGE_SECTION_HEADER;
      ReadByOffset(section, this->dos_header->e_lfanew + sizeof(IMAGE_NT_HEADERS) + i * sizeof(IMAGE_SECTION_HEADER));
      if (strcmp((char*)section->Name, (char*)".rdata") == 0) {
        this->rdata_offset = section->PointerToRawData;
        this->rdata_size = section->SizeOfRawData;
        this->rdata_virtual_shift = section->VirtualAddress - section->PointerToRawData;
        break;
      }
      delete section;
    }

    if (!this->rdata_offset) {
      std::cerr << "unable to find rdata section\n";
      exit(2);
    }
  }

  ~PEParser()
  {
    delete this->dos_header;
    delete this->nt_headers;
    fclose(this->file);
  }

  std::optional<uintptr_t> FindString(const std::string& str)
  {
    auto length = str.size() + 1;
    std::vector<char> target(str.begin(), str.begin() + length);
    auto it = 0;
    while (it < this->rdata_size) {
      std::vector<char> buf(length);
      ReadByOffset(buf.data(), this->rdata_offset + it, length);
      if (buf == target) {
        return this->rdata_offset + it + this->rdata_virtual_shift;
      }
      it += 8;
    }
    return std::nullopt;
  }

  template <typename T>
  void ReadByOffset(T* dst, uintptr_t offset, size_t count = 1)
  {
    rewind(this->file);
    fseek(this->file, offset, SEEK_SET);
    fread(dst, sizeof(T), count, this->file);
  }

private:
  PEParser() {}
  PEParser(const PEParser&) = delete;
  PEParser(PEParser&&) = delete;

  FILE* file;
  const char* filename;
  IMAGE_DOS_HEADER* dos_header = new IMAGE_DOS_HEADER;
  IMAGE_NT_HEADERS64* nt_headers = new IMAGE_NT_HEADERS64;
  uintptr_t rdata_offset;
  uintptr_t rdata_size;
  uintptr_t rdata_virtual_shift;
};