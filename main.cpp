#include <irrlicht/irrlicht.h>
#include <stdexcept>
#include <iostream>
#include <memory>
#include <vector>

//Esta en una funcion lambda
//Agregamos otra linea
//auto des = [](irr::IrrlichtDevice*p){p->drop();};


struct TheEngine
{
    using u32 = irr::u32;
    TheEngine(u32 const w, u32 const h):width_(w),height_(h){
    
    if (!device_) throw std::runtime_error("Couldn't initialize Irrlicht Device");
    smgr_->addCameraSceneNodeFPS();
    }

    bool run() const {return device_->run();}
    void addStaticText(){
        guienv_->addStaticText(L"Hello World! This is the Irrlicht Software renderer!",
        irr::core::rect<irr::s32>(10,10,260,22), true);
    }

    void beginScene(){
        driver_->beginScene(true, true, irr::video::SColor(255,100,101,140));
    }

    void endScene(){
        driver_->endScene();
    }

    void drawAll(){
        smgr_->drawAll();
        guienv_->drawAll();
    }

    auto* createSphere(){
        auto * node = smgr_->addSphereSceneNode();
        if (!node) throw std::runtime_error("Couldn't create sphere");
        auto* textura = driver_->getTexture("media/wall.bmp");
        if(!textura) throw std::runtime_error("Couldn't create sphere");

        node->setPosition(irr::core::vector3df(0,0,30));
        
        node->setMaterialTexture(0, textura);
        node->setMaterialFlag(irr::video::EMF_LIGHTING, false);

        return node;

    }
   
    private:
    irr::u32 width_{},height_{};
    static void Destroy(irr::IrrlichtDevice *p){ std::cerr<<"Me LLAmo\n"; p->drop();}
    using Destructfunc = void(*)(irr::IrrlichtDevice*);
    using irrDeviceManaged = std::unique_ptr<irr::IrrlichtDevice,Destructfunc> ;

    
    irrDeviceManaged device_{irr::createDevice( irr::video::EDT_SOFTWARE, irr::core::dimension2d<irr::u32>(width_,height_), 16,
            false, false, false, 0),Destroy};

    irr::video::IVideoDriver  * const driver_ { device_ ? device_->getVideoDriver()    :nullptr};
    irr::scene::ISceneManager * const smgr_   { device_ ? device_->getSceneManager()   :nullptr};
    irr::gui::IGUIEnvironment * const guienv_ { device_ ? device_->getGUIEnvironment() :nullptr};
    
};

template <typename Type>
struct EntityManager
{
    using TypeProcessFunc = void (*)(Type&);

    EntityManager(size_t defaultSize = 100){
        //entities_.reserve(100);
    }

    auto & createEntity(){ return entities_.emplace_back();}
    void forall(TypeProcessFunc process){
        for (auto &e : entities_)
        {
            process(e);
        }
        
    }
    private:
    std::vector<Type> entities_;
};

struct PhysicsComponent 
{
    float x{},y{},z{};
    float vx{},vy{},vz{};
};

struct RenderComponent
{
    irr::scene::ISceneNode * node;
};


struct Entity
{
    
    PhysicsComponent physics;
    RenderComponent render;
};

struct PhysicSystem


{
    void update(EntityManager<Entity> &EM){
        EM.forall([](Entity& e){
            e.physics.x += e.physics.vx;
            e.physics.y += e.physics.vy;
            e.physics.z += e.physics.vy;
        });
    
    
    }
};

struct RenderSystem
{
    void update(EntityManager<Entity> &EM, TheEngine &GFX){

       
        EM.forall([](Entity& e){
            auto & phy {e.physics};
            //irr::core::vector3df v (e.physics.x,e.physics.y,e.physics.z);
            //e.render.node ->setPosition(v);
            e.render.node->setPosition({phy.x,phy.y,phy.z});
        });

        GFX.beginScene();
        GFX.drawAll();
        GFX.endScene();

    }
};




void Game(){
    TheEngine dev{640,480};
    EntityManager<Entity> EM;
    PhysicSystem PhySys;
    RenderSystem RenSys;

    auto& e1 = EM.createEntity();
    
    e1.render.node = dev.createSphere();
    e1.physics.z=10.0f;
    e1.physics.vz = 0.2f;

    while (dev.run())
    {
        PhySys.update(EM);
        RenSys.update(EM,dev);
    }
    
    /*
    TheEngine dev{640,480};
    dev.addStaticText();

     while (dev.run())
     {
        dev.beginScene();
        
        dev.drawAll();
        dev.endScene();
    }
    */
}
     


int main(){
  
    try
    {
        Game();
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    
    return 0;
    
}
