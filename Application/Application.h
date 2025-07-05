#pragma once
#include <memory>

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

        std::unique_ptr<IWindow> window; ///< Application window
        std::unique_ptr<IRenderer> renderer; ///< Main renderer

    private:
        Application(const Application&) = delete;
        Application& operator=(const Application&) = delete;
        bool isRunning = true;
    };
}
