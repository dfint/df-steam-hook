class StateManager {
public:
  [[nodiscard]] static StateManager* GetSingleton() {
    static StateManager singleton;
    return &singleton;
  }

  enum GameState {
    Init,
    Menu,
    Loading,
    Game
  };

  void State(StateManager::GameState state) {
    if (this->state.load() != state) {
      auto it = this->callbacks.find(state);
      if (it != this->callbacks.end()) {
        it->second();
      }
    }
    this->state.store(state);
  }

  void SetCallback(StateManager::GameState state, std::function<void()> callback) {
    this->callbacks.insert_or_assign(state, callback);
  }

private:
  StateManager() {}
  StateManager(const StateManager&) = delete;
  StateManager(StateManager&&) = delete;

  ~StateManager() {
    callbacks.clear();
    delete this;
  };

  std::unordered_map<StateManager::GameState, std::function<void()>> callbacks;
  std::atomic<GameState> state{ StateManager::Init };
};