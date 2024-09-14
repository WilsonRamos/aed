#include <irrlicht/irrlicht.h>
#include <stdexcept>
#include <iostream>
#include <functional>
#include <vector>

struct TheEngine {
    using u32 = irr::u32;
    TheEngine(u32 const w, u32 const h) : width_(w), height_(h) {
        device_ = irr::createDevice(irr::video::EDT_SOFTWARE, irr::core::dimension2d<irr::u32>(width_, height_), 16,
            false, false, false, 0);
        if (!device_) throw std::runtime_error("Couldn't initialize Irrlicht Device");
        smgr_ = device_->getSceneManager();
        driver_ = device_->getVideoDriver();
        guienv_ = device_->getGUIEnvironment();
        smgr_->addCameraSceneNodeFPS();
    }

    bool run() const { return device_->run(); }
    void addStaticText() {
        guienv_->addStaticText(L"Hello World! This is the Irrlicht Software renderer!",
            irr::core::rect<irr::s32>(10, 10, 260, 22), true);
    }

    void beginScene() {
        driver_->beginScene(true, true, irr::video::SColor(255, 100, 101, 140));
    }

    void endScene() {
        driver_->endScene();
    }

    void drawAll() {
        smgr_->drawAll();
        guienv_->drawAll();
    }

    auto* createSphere() {
        auto* node = smgr_->addSphereSceneNode();
        if (!node) throw std::runtime_error("Couldn't create sphere");
        auto* textura = driver_->getTexture("media/wall.bmp");
        if (!textura) throw std::runtime_error("Couldn't load texture");

        node->setPosition(irr::core::vector3df(0, 0, 30)); // Move the sphere to position (0, 0, 30)
        node->setMaterialTexture(0, textura);
        node->setMaterialFlag(irr::video::EMF_LIGHTING, false);

        return node;
    }

private:
    irr::u32 width_{}, height_{};
    irr::IrrlichtDevice* device_{};
    irr::video::IVideoDriver* driver_{};
    irr::scene::ISceneManager* smgr_{};
    irr::gui::IGUIEnvironment* guienv_{};
};

struct PhysicsComponent {
    float x{}, y{}, z{};
    float vx{}, vy{}, vz{};
};

struct RenderComponent {
    irr::scene::ISceneNode* node;
};

struct Entity {
    PhysicsComponent physics;
    RenderComponent render;
};

template<typename Type>
struct EntityManager {
    using TypeProcessFunc = std::function<void(Type&)>;

    auto& createEntity() { return entities_.emplace_back(); }
    void forall(TypeProcessFunc process) {
        for (auto& e : entities_) {
            process(e);
        }
    }

private:
    std::vector<Type> entities_;
};

struct PhysicSystem {
    void update(EntityManager<Entity>& EM) {
        EM.forall([](Entity& e) {
            e.physics.x += e.physics.vx;
            e.physics.y += e.physics.vy;
            e.physics.z += e.physics.vz;
        });
    }
};

struct RenderSystem {
    void update(EntityManager<Entity>& EM, TheEngine& GFX) {
        EM.forall([](Entity& e) {
            auto& phy{ e.physics };
            e.render.node->setPosition({ phy.x, phy.y, phy.z });
        });

        GFX.beginScene();
        GFX.drawAll();
        GFX.endScene();
    }
};

void Game() {
    TheEngine dev{ 640,480 };
    EntityManager<Entity> EM;
    PhysicSystem PhySys;
    RenderSystem RenSys;
    auto& e1 = EM.createEntity();
    e1.render.node = dev.createSphere();
    e1.physics.z = 30.0f; // Starting further away from the camera
    e1.physics.vz = 0.2f;

    while (dev.run()) {
        PhySys.update(EM);
        RenSys.update(EM, dev);
    }
}

int main() {
    try {
        Game();
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << '\n';
    }

    return 0;
}
