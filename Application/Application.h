#pragma once
#include <memory>
#include <string>

class IWindow;
class IRenderer;

namespace Engine {

    /**
     * Base class for the engine application.
     * Defines the lifecycle: initialization, main loop, shutdown.
     */
    class Application {
    public:
        Application();
        Application(std::string title, int width, int height);
        virtual ~Application();

        // Starts the main application loop
        void Run();

    protected:
        // Initializes application resources
        virtual void Init();
        // Updates application state (called every frame)
        virtual void Update(float dt);
        // Shuts down the application and releases resources
        virtual void Shutdown();

        std::unique_ptr<IWindow> window;
        std::unique_ptr<IRenderer> renderer;

    private:
        Application(const Application&) = delete;
        Application& operator=(const Application&) = delete;

        void MainLoop();
        bool isRunning = true;
        std::string appName = "Engine Application";
        std::string appVersion = "1.0.0";
    };
}
