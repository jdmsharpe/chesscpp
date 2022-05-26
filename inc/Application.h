#ifndef APPLICATION_H
#define APPLICATION_H

#include "Window.h"

enum class AppState {
  UNKNOWN = -1,
  MENU = 0,
  GAME_IN_PROGRESS = 1,
  GAME_COMPLETE = 2
};

class Application {
public:
  Application(int argc, char **argv);
  virtual ~Application() = default;

  // Should not be moved or copied
  Application(const Application &) = delete;
  Application(Application &) = delete;
  Application(const Application &&) = delete;
  Application(Application &&) = delete;

  int run();

private:
  std::unique_ptr<Window> m_window = nullptr;
  AppState m_appState = AppState::UNKNOWN;

  bool m_legacyMode = false;
};

#endif // APPLICATION_H
