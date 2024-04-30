#include "Common.h"
#include "OpenGLCommon.h"
#include "OpenGLMarkerObjects.h"
#include "OpenGLBgEffect.h"
#include "OpenGLMesh.h"
#include "OpenGLViewer.h"
#include "OpenGLWindow.h"
#include "TinyObjLoader.h"
#include "OpenGLSkybox.h"
#include <algorithm>
#include <iostream>
#include <random>
#include <unordered_set>
#include <vector>
#include <string>


#ifndef __Main_cpp__
#define __Main_cpp__

#ifdef __APPLE__
#define CLOCKS_PER_SEC 100000
#endif

using namespace OpenGLFbos;

class MyDriver : public OpenGLViewer
{
    std::vector<OpenGLTriangleMesh *> mesh_object_array;
    OpenGLBgEffect *bgEffect = nullptr;
    OpenGLSkybox *skybox = nullptr;
    clock_t startTime;

public:
    virtual void Initialize()
    {
        draw_axes = false;
        startTime = clock();
        OpenGLViewer::Initialize();
    }

    Vector3f RandomPosition(const Vector3f& minPosition, const Vector3f& maxPosition) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<float> disX(minPosition.x(), maxPosition.x());
        std::uniform_real_distribution<float> disY(minPosition.y(), maxPosition.y());
        std::uniform_real_distribution<float> disZ(minPosition.z(), maxPosition.z());
        float x = disX(gen);
        float y = disY(gen);
        float z = disZ(gen);
        return Vector3f(x, y, z);
    }

    virtual void Initialize_Data()
    {
        //// Load all the shaders you need for the scene 
        //// In the function call of Add_Shader_From_File(), we specify three names: 
        //// (1) vertex shader file name
        //// (2) fragment shader file name
        //// (3) shader name used in the shader library
        //// When we bind a shader to an object, we implement it as follows:
        //// object->Add_Shader_Program(OpenGLShaderLibrary::Get_Shader("shader_name"));
        //// Here "shader_name" needs to be one of the shader names you created previously with Add_Shader_From_File()
        OpenGLShaderLibrary::Instance()->Add_Shader_From_File("shaders/grass.vert", "shaders/grass.frag", "grass");
        OpenGLShaderLibrary::Instance()->Add_Shader_From_File("shaders/basic.vert", "shaders/basic.frag", "basic");
        OpenGLShaderLibrary::Instance()->Add_Shader_From_File("shaders/basic.vert", "shaders/phong.frag", "phong");
        OpenGLShaderLibrary::Instance()->Add_Shader_From_File("shaders/toon.vert", "shaders/faceshader.frag", "faceshader");
        OpenGLShaderLibrary::Instance()->Add_Shader_From_File("shaders/basic.vert", "shaders/bodyshader.frag", "bodyshader");
        OpenGLShaderLibrary::Instance()->Add_Shader_From_File("shaders/basic.vert", "shaders/clothshader.frag", "clothshader");
        //OpenGLShaderLibrary::Instance()->Add_Shader_From_File("shaders/basic.vert", "shaders/hairshader.frag", "hairshader");

        OpenGLShaderLibrary::Instance()->Add_Shader_From_File("shaders/basic.vert", "shaders/environment.frag", "environment");
        //OpenGLShaderLibrary::Instance()->Add_Shader_From_File("shaders/stars.vert", "shaders/stars.frag", "stars");
        OpenGLShaderLibrary::Instance()->Add_Shader_From_File("shaders/basic.vert", "shaders/alphablend.frag", "blend");
        OpenGLShaderLibrary::Instance()->Add_Shader_From_File("shaders/billboard.vert", "shaders/alphablend.frag", "billboard");
        OpenGLShaderLibrary::Instance()->Add_Shader_From_File("shaders/terrain.vert", "shaders/terrain.frag", "terrain");
        OpenGLShaderLibrary::Instance()->Add_Shader_From_File("shaders/skybox.vert", "shaders/skybox.frag", "skybox");

        OpenGLShaderLibrary::Instance()->Add_Shader_From_File("shaders/basic.vert", "shaders/multiplyblend.frag", "multiply");

        //// Load all the textures you need for the scene
        //// In the function call of Add_Shader_From_File(), we specify two names:
        //// (1) the texture's file name
        //// (2) the texture used in the texture library
        //// When we bind a texture to an object, we implement it as follows:
        //// object->Add_Texture("tex_sampler", OpenGLTextureLibrary::Get_Texture("tex_name"));
        //// Here "tex_sampler" is the name of the texture sampler2D you used in your shader, and
        //// "tex_name" needs to be one of the texture names you created previously with Add_Texture_From_File()

        /*
        OpenGLTextureLibrary::Instance()->Add_Texture_From_File("tex/earth_color.png", "sphere_color");
        OpenGLTextureLibrary::Instance()->Add_Texture_From_File("tex/earth_normal.png", "sphere_normal");
        OpenGLTextureLibrary::Instance()->Add_Texture_From_File("tex/window.png", "window_color");
        OpenGLTextureLibrary::Instance()->Add_Texture_From_File("tex/buzz_color.png", "buzz_color");
        OpenGLTextureLibrary::Instance()->Add_Texture_From_File("tex/star.png", "star_color");
        */
        OpenGLTextureLibrary::Instance()->Add_Texture_From_File("tex/star.png", "star_color");

        OpenGLTextureLibrary::Instance()->Add_Texture_From_File("tex/skybox.png", "skybox");

        OpenGLTextureLibrary::Instance()->Add_Texture_From_File("tex/head_color.png", "head_color");
        OpenGLTextureLibrary::Instance()->Add_Texture_From_File("tex/head_sdf.png", "head_sdf");
        OpenGLTextureLibrary::Instance()->Add_Texture_From_File("tex/head_st.png", "head_st");
        OpenGLTextureLibrary::Instance()->Add_Texture_From_File("tex/head_rd.png", "head_rd");
        OpenGLTextureLibrary::Instance()->Add_Texture_From_File("tex/head_M.png", "head_M");
        OpenGLTextureLibrary::Instance()->Add_Texture_From_File("tex/hair_color.png", "hair_color");
        OpenGLTextureLibrary::Instance()->Add_Texture_From_File("tex/hair_normal.png", "hair_normal");
        OpenGLTextureLibrary::Instance()->Add_Texture_From_File("tex/cloth_color.png", "cloth_color");
        OpenGLTextureLibrary::Instance()->Add_Texture_From_File("tex/cloth_normal.png", "cloth_normal");
        OpenGLTextureLibrary::Instance()->Add_Texture_From_File("tex/cloth_metal.png", "cloth_metal");
        OpenGLTextureLibrary::Instance()->Add_Texture_From_File("tex/cloth_emission.png", "cloth_emission");
        OpenGLTextureLibrary::Instance()->Add_Texture_From_File("tex/hair_metal.png", "hair_metal");
        OpenGLTextureLibrary::Instance()->Add_Texture_From_File("tex/hair_metal.png", "hair_rs");
        OpenGLTextureLibrary::Instance()->Add_Texture_From_File("tex/body_color.png", "body_color");
        OpenGLTextureLibrary::Instance()->Add_Texture_From_File("tex/eye_color.png", "eye_color");
        OpenGLTextureLibrary::Instance()->Add_Texture_From_File("tex/no_normal.jpg", "basic_normal");
        
        OpenGLTextureLibrary::Instance()->Add_Texture_From_File("tex/eyeshadow_multiply.png", "eyeshadow_multiply");
        OpenGLTextureLibrary::Instance()->Add_Texture_From_File("tex/hairshadow_multiply.png", "hairshadow_multiply");
        OpenGLTextureLibrary::Instance()->Add_Texture_From_File("tex/grass.jpg", "grass_color");
        OpenGLTextureLibrary::Instance()->Add_Texture_From_File("tex/grass_black.jpg", "grass_black");
        OpenGLTextureLibrary::Instance()->Add_Texture_From_File("tex/grass.png", "grass");
        OpenGLTextureLibrary::Instance()->Add_Texture_From_File("tex/grass2.png", "grass2");





        //// Add all the lights you need for the scene (no more than 4 lights)
        //// The four parameters are position, ambient, diffuse, and specular.
        //// The lights you declared here will be synchronized to all shaders in uniform lights.
        //// You may access these lights using lt[0].pos, lt[1].amb, lt[1].dif, etc.
        //// You can also create your own lights by directly declaring them in a shader without using Add_Light().
        //// Here we declared three default lights for you. Feel free to add/delete/change them at your will.

        opengl_window->Add_Light(Vector3f(-3, 1, 3), Vector3f(0.1, 0.1, 0.1), Vector3f(0.9, 0.9, 0.9), Vector3f(0.5, 0.5, 0.5));
        opengl_window->Add_Light(Vector3f(0, 0, -5), Vector3f(0.1, 0.1, 0.1), Vector3f(0.9, 0.9, 0.9), Vector3f(0.5, 0.5, 0.5));
        //opengl_window->Add_Light(Vector3f(5, 1, 3), Vector3f(0.1, 0.1, 0.1), Vector3f(0.9, 0.9, 0.9), Vector3f(0.5, 0.5, 0.5));

        //// Add the background / environment
        //// Here we provide you with four default options to create the background of your scene:
        //// (1) Gradient color (like A1 and A2; if you want a simple background, use this one)
        //// (2) Programmable Canvas (like A7 and A8; if you consider implementing noise or particles for the background, use this one)
        //// (3) Sky box (cubemap; if you want to load six background images for a skybox, use this one)
        //// (4) Sky sphere (if you want to implement a sky sphere, enlarge the size of the sphere to make it colver the entire scene and update its shaders for texture colors)
        //// By default, Option (2) (Buzz stars) is turned on, and all the other three are commented out.
        
        //// Background Option (1): Gradient color
        /*
        {
            auto bg = Add_Interactive_Object<OpenGLBackground>();
            bg->Set_Color(OpenGLColor(0.1f, 0.1f, 0.1f, 1.f), OpenGLColor(0.3f, 0.1f, .1f, 1.f));
            bg->Initialize();
        }
        */

        //// Background Option (2): Programmable Canvas
        //// By default, we load a GT buzz + a number of stars
        
        //{
        //    bgEffect = Add_Interactive_Object<OpenGLBgEffect>();
        //    bgEffect->Add_Shader_Program(OpenGLShaderLibrary::Get_Shader("stars"));
        //    bgEffect->Add_Texture("tex_buzz", OpenGLTextureLibrary::Get_Texture("buzz_color")); // bgEffect can also Add_Texture
        //    bgEffect->Initialize();
        //}

        
        //// Background Option (3): Sky box
        //// Here we provide a default implementation of a sky box; customize it for your own sky box
        //{
        //    // from https://www.humus.name/index.php?page=Textures
        //    const std::vector<std::string> cubemap_files{
        //        "cubemap/posx.jpg",     //// + X
        //        "cubemap/negx.jpg",     //// - X
        //        "cubemap/posy.jpg",     //// + Y
        //        "cubemap/negy.jpg",     //// - Y
        //        "cubemap/posz.jpg",     //// + Z
        //        "cubemap/negz.jpg",     //// - Z 
        //    };
        //    OpenGLTextureLibrary::Instance()->Add_CubeMap_From_Files(cubemap_files, "cube_map");

        //    skybox = Add_Interactive_Object<OpenGLSkybox>();
        //    skybox->Add_Shader_Program(OpenGLShaderLibrary::Get_Shader("skybox"));
        //    skybox->Initialize();
        //}

        //// Background Option (4): Sky sphere
        //// Here we provide a default implementation of a textured sphere; customize it for your own sky sphere
       
        {
            //// create object by reading an obj mesh
            auto sphere = Add_Obj_Mesh_Object("obj/sphere.obj");

            //// set object's transform
            Matrix4f t;
            t << 20, 0, 0, 0,
                0, 20, 0, 0,
                0, 0, 20, 0,
                0, 0, 0, 1;
            sphere->Set_Model_Matrix(t);

            //// set object's material
            // sphere->Set_Ka(Vector3f(0.1, 0.1, 0.1));
            // sphere->Set_Kd(Vector3f(0.7, 0.7, 0.7));
            // sphere->Set_Ks(Vector3f(2, 2, 2));
            // sphere->Set_Shininess(128);

            //// bind texture to object
            sphere->Add_Texture("tex_color", OpenGLTextureLibrary::Get_Texture("skybox"));
            //sphere->Add_Texture("tex_normal", OpenGLTextureLibrary::Get_Texture("basic_normal"));

            //// bind shader to object
            sphere->Add_Shader_Program(OpenGLShaderLibrary::Get_Shader("basic"));
        }

        std::random_device rd; 
        std::mt19937 gen(rd()); 
        std::uniform_real_distribution<> distribX(-3.0, 3.0);
        std::uniform_real_distribution<> distribY(-3.0, 3.0);

        for (int i = 0; i < 100; i++) {
            float x = distribX(gen);  
            float y = distribY(gen);  

            create_grass_bundle(x, y);
        }

        // {
        //     //// create object by reading an obj mesh
        //     auto grass = Add_Obj_Mesh_Object("obj/grass.obj");

        //     //// set object's transform
        //     Matrix4f t;
        //     float scaleFactor = 0.001; // Adjust this value to control the scale
        //     t << scaleFactor, 0, 0, 0,
        //     0, scaleFactor, 0, 0,
        //     0, 0, scaleFactor, 0,
        //     0, 0, 0, 1; // 4x4 scaling matrix
        //     grass->Set_Model_Matrix(t);


        //     //// bind texture to object
        //     grass->Add_Texture("tex_color", OpenGLTextureLibrary::Get_Texture("grass"));
        //     //grass->Add_Texture("tex_color", OpenGLTextureLibrary::Get_Texture("grass_black"));

        //     //// bind shader to object
        //     grass->Add_Shader_Program(OpenGLShaderLibrary::Get_Shader("alphablend"));
        // }
        // Define the area where you want to scatter the grass
    //Vector3f minPosition = {-2.0f, 0.0f, -2.0f}; // Minimum position
    //Vector3f maxPosition = {2.0f, 0.0f, 2.0f};   // Maximum position
    //int numInstances = 100; // Number of grass instances

    //// Loop to create and scatter grass instances
    //for (int i = 0; i < numInstances; ++i) {
    //// Create a new grass object instance
    //    auto grass = Add_Obj_Mesh_Object("obj/grass.obj");

    //// Generate random position within the specified area
    //    Vector3f position = RandomPosition(minPosition, maxPosition);

    //// Set the scale of the grass object
    //    Matrix4f t;
    //    float scaleFactor = 0.001; // Adjust this value to control the scale
    //    t << scaleFactor, 0, 0, 0,
    //        0, scaleFactor, 0, 0,
    //        0, 0, scaleFactor, 0,
    //        0, 0, 0, 1; // 4x4 scaling matrix
    //    grass->Set_Model_Matrix(t);

    //// Set the position of the grass object
    //    Matrix4f translationMatrix = Matrix4f::Identity();
    //    translationMatrix.block<3,1>(0,3) = position;
    //    grass->Set_Model_Matrix(translationMatrix * t);

    //// Bind texture to the grass object
    //    grass->Add_Texture("tex_color", OpenGLTextureLibrary::Get_Texture("grass"));

    //// Bind shader to the grass object
    //    grass->Add_Shader_Program(OpenGLShaderLibrary::Get_Shader("grass"));
    //}
    //   

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////// Angelina Character Import ////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        //////////////////////////////////////////////////////Normal OBJ import/////////////////////////////////////////////////////////////
        ////head
        {
            //// create object by reading an obj mesh
            auto head = Add_Obj_Mesh_Object("obj/head.obj");

            //// set object's transform
            Matrix4f t;
            t << 1, 0, 0, 0,
                0, 1, 0, 0,
                0, 0, 1, 0,
                0, 0, 0, 1;
            head->Set_Model_Matrix(t);

            //// set object's material
            head->Set_Ka(Vector3f(0.1, 0.1, 0.1));
            head->Set_Kd(Vector3f(0.7, 0.7, 0.7));
            head->Set_Ks(Vector3f(2, 2, 2));
            head->Set_Shininess(128);

            //// bind texture to object
            head->Add_Texture("tex_color", OpenGLTextureLibrary::Get_Texture("head_color"));
            head->Add_Texture("tex_sdf", OpenGLTextureLibrary::Get_Texture("head_sdf"));
            head->Add_Texture("tex_st", OpenGLTextureLibrary::Get_Texture("head_st"));
            head->Add_Texture("tex_rd", OpenGLTextureLibrary::Get_Texture("head_rd"));
            head->Add_Texture("tex_M", OpenGLTextureLibrary::Get_Texture("head_M"));

            //// bind shader to object
            head->Add_Shader_Program(OpenGLShaderLibrary::Get_Shader("faceshader"));
        }

        //hair
        {
            auto hair = Add_Obj_Mesh_Object("obj/hair.obj");
            Matrix4f t;
            t << 1, 0, 0, 0,
                0, 1, 0, 0,
                0, 0, 1, 0,
                0, 0, 0, 1;
            hair->Set_Model_Matrix(t);
            hair->Set_Ka(Vector3f(0.1, 0.1, 0.1));
            hair->Set_Kd(Vector3f(0.7, 0.7, 0.7));
            hair->Set_Ks(Vector3f(2, 2, 2));
            hair->Set_Shininess(128);
            hair->Add_Texture("tex_color", OpenGLTextureLibrary::Get_Texture("hair_color"));
            hair->Add_Texture("tex_normal", OpenGLTextureLibrary::Get_Texture("hair_normal"));
            //hair->Add_Texture("tex_metal", OpenGLTextureLibrary::Get_Texture("hair_metal"));
            //hair->Add_Texture("tex_st", OpenGLTextureLibrary::Get_Texture("hair_ST"));
            hair->Add_Texture("tex_rs", OpenGLTextureLibrary::Get_Texture("hair_rs"));
            hair->Add_Shader_Program(OpenGLShaderLibrary::Get_Shader("phong"));
        }
        //cloth
        {
            auto cloth = Add_Obj_Mesh_Object("obj/cloth.obj");
            Matrix4f t;
            t << 1, 0, 0, 0,
                0, 1, 0, 0,
                0, 0, 1, 0,
                0, 0, 0, 1;
            cloth->Set_Model_Matrix(t);
            cloth->Set_Ka(Vector3f(0.1, 0.1, 0.1));
            cloth->Set_Kd(Vector3f(0.7, 0.7, 0.7));
            cloth->Set_Ks(Vector3f(2, 2, 2));
            cloth->Set_Shininess(128);
            cloth->Add_Texture("tex_color", OpenGLTextureLibrary::Get_Texture("cloth_color"));
            cloth->Add_Texture("tex_normal", OpenGLTextureLibrary::Get_Texture("cloth_normal"));
            cloth->Add_Texture("tex_metal", OpenGLTextureLibrary::Get_Texture("cloth_metal"));
            cloth->Add_Texture("tex_emission", OpenGLTextureLibrary::Get_Texture("cloth_emission"));
            cloth->Add_Shader_Program(OpenGLShaderLibrary::Get_Shader("clothshader"));
        }
        //body
        {
            auto body = Add_Obj_Mesh_Object("obj/body.obj");
            Matrix4f t;
            t << 1, 0, 0, 0,
                0, 1, 0, 0,
                0, 0, 1, 0,
                0, 0, 0, 1;
            body->Set_Model_Matrix(t);
            body->Set_Ka(Vector3f(0.1, 0.1, 0.1));
            body->Set_Kd(Vector3f(0.7, 0.7, 0.7));
            body->Set_Ks(Vector3f(2, 2, 2));
            body->Set_Shininess(128);
            body->Add_Texture("tex_color", OpenGLTextureLibrary::Get_Texture("body_color"));
            body->Add_Texture("tex_rd", OpenGLTextureLibrary::Get_Texture("head_rd"));
            body->Add_Shader_Program(OpenGLShaderLibrary::Get_Shader("bodyshader"));
        }
        //eyebrow
        {
            auto eyebrow = Add_Obj_Mesh_Object("obj/eyebrow.obj");
            Matrix4f t;
            t << 1, 0, 0, 0,
                0, 1, 0, 0,
                0, 0, 1, 0,
                0, 0, 0, 1;
            eyebrow->Set_Model_Matrix(t);
            eyebrow->Set_Ka(Vector3f(0.1, 0.1, 0.1));
            eyebrow->Set_Kd(Vector3f(0.7, 0.7, 0.7));
            eyebrow->Set_Ks(Vector3f(2, 2, 2));
            eyebrow->Set_Shininess(128);
            eyebrow->Add_Texture("tex_color", OpenGLTextureLibrary::Get_Texture("head_color"));
            eyebrow->Add_Texture("tex_normal", OpenGLTextureLibrary::Get_Texture("basic_normal"));
            eyebrow->Add_Shader_Program(OpenGLShaderLibrary::Get_Shader("phong"));
        }
        //eye
        {
            auto eye = Add_Obj_Mesh_Object("obj/eyeball.obj");
            Matrix4f t;
            t << 1, 0, 0, 0,
                0, 1, 0, 0,
                0, 0, 1, 0,
                0, 0, 0, 1;
            eye->Set_Model_Matrix(t);
            eye->Set_Ka(Vector3f(0.1, 0.1, 0.1));
            eye->Set_Kd(Vector3f(0.7, 0.7, 0.7));
            eye->Set_Ks(Vector3f(2, 2, 2));
            eye->Set_Shininess(128);
            eye->Add_Texture("tex_color", OpenGLTextureLibrary::Get_Texture("eye_color"));
            eye->Add_Texture("tex_normal", OpenGLTextureLibrary::Get_Texture("basic_normal"));
            eye->Add_Shader_Program(OpenGLShaderLibrary::Get_Shader("phong"));
        }

        //////////////////////////////////////////////////////Special OBJ import/////////////////////////////////////////////////////////////
        //fur - placeholder
        //TO-DO: ������ͼë����ͼʵ��ë��Ч����Ӧ�úͲݲ�࣬�Ȳ�д���������
        //{
        //    auto fur = Add_Obj_Mesh_Object("obj/fur.obj");
        //    Matrix4f t;
        //    t << 1, 0, 0, 0,
        //        0, 1, 0, 0,
        //        0, 0, 1, 0,
        //        0, 0, 0, 1;
        //    fur->Set_Model_Matrix(t);
        //    fur->Set_Ka(Vector3f(0.1, 0.1, 0.1));
        //    fur->Set_Kd(Vector3f(0.7, 0.7, 0.7));
        //    fur->Set_Ks(Vector3f(2, 2, 2));
        //    fur->Set_Shininess(128);
        //    fur->Add_Texture("tex_color", OpenGLTextureLibrary::Get_Texture("hair_color"));
        //    fur->Add_Texture("tex_normal", OpenGLTextureLibrary::Get_Texture("hair_normal"));
        //    fur->Add_Shader_Program(OpenGLShaderLibrary::Get_Shader("phong"));
        //}

        //auto shadowFbo = OpenGLFbos::Get_Fbo_Instance("shadowFbo", 0);



        //eyeshadow - ���ǻ�ױ�Ǹ���Ӱ�����ۿ����������Ӱ
        //Ӧ������Ƭ����֮��ģ���Ϊ��Ӧ��texture�ǡ�M��
        //{
        //    auto eyeshadow = Add_Obj_Mesh_Object("obj/eyeshadow.obj");
        //    Matrix4f t;
        //    t << 1, 0, 0, 0,
        //        0, 1, 0, 0,
        //        0, 0, 1, 0,
        //        0, 0, 0, 1;
        //    eyeshadow->Set_Model_Matrix(t);
        //    eyeshadow->Set_Ka(Vector3f(0.1, 0.1, 0.1));
        //    eyeshadow->Set_Kd(Vector3f(0.7, 0.7, 0.7));
        //    eyeshadow->Set_Ks(Vector3f(2, 2, 2));
        //    eyeshadow->Set_Shininess(128);
        //    eyeshadow->Add_Texture("tex_color", OpenGLTextureLibrary::Get_Texture("eyeshadow_multiply"));
        //    eyeshadow->Add_Texture("tex_normal", OpenGLTextureLibrary::Get_Texture("basic_normal"));
        //    eyeshadow->Add_Shader_Program(OpenGLShaderLibrary::Get_Shader("phong"));
        //}
        //��eyeshadowͬ��
        //{
        //    auto hairshadow = Add_Obj_Mesh_Object("obj/hairshadow.obj");
        //    Matrix4f t;
        //    t << 1, 0, 0, 0,
        //        0, 1, 0, 0,
        //        0, 0, 1, 0,
        //        0, 0, 0, 1;
        //    hairshadow->Set_Model_Matrix(t);
        //    hairshadow->Set_Ka(Vector3f(0.1, 0.1, 0.1));
        //    hairshadow->Set_Kd(Vector3f(0.7, 0.7, 0.7));
        //    hairshadow->Set_Ks(Vector3f(2, 2, 2));
        //    hairshadow->Set_Shininess(128);
        //    hairshadow->Add_Texture("tex_color", OpenGLTextureLibrary::Get_Texture("hairshadow_multiply"));
        //    hairshadow->Add_Texture("tex_normal", OpenGLTextureLibrary::Get_Texture("basic_normal"));
        //    hairshadow->Add_Shader_Program(OpenGLShaderLibrary::Get_Shader("phong"));
        //}



        //// Here we show an example of adding a mesh with noise-terrain (A6)
        {
           //// create object by reading an obj mesh
           auto terrain = Add_Obj_Mesh_Object("obj/plane.obj");

           // set object's transform
        //    Matrix4f r, s, t;
        //    r << 1, 0, 0, 0,
        //        0, 0.5, 0.67, 0,
        //        0, -0.67, 0.5, 0,
        //        0, 0, 0, 1;
        //    s << 0.5, 0, 0, 0,
        //        0, 0.5, 0, 0,
        //        0, 0, 0.5, 0,
        //        0, 0, 0, 1;
        //    t << 1, 0, 0, -2,
        //         0, 1, 0, 0.5,
        //         0, 0, 1, 0,
        //         0, 0, 0, 1,
        //    terrain->Set_Model_Matrix(t * s * r);
        Matrix4f t;
           t << 1, 0, 0, 0,
                0, 1, 0, 0,
                0, 0, 1, 2.5,
                0, 0, 0, 1;
           terrain->Set_Model_Matrix(t);

           //// set object's material
           terrain->Set_Ka(Vector3f(0.1f, 0.1f, 0.1f));
           terrain->Set_Kd(Vector3f(0.7f, 0.7f, 0.7f));
           terrain->Set_Ks(Vector3f(1, 1, 1));
           terrain->Set_Shininess(128.f);

           //// bind shader to object (we do not bind texture for this object because we create noise for texture)
           terrain->Add_Shader_Program(OpenGLShaderLibrary::Get_Shader("terrain"));
        }

        //// Here we show an example of adding a transparent object with alpha blending
        //// This example will be useful if you implement objects such as tree leaves, grass blades, flower pedals, etc.
        //// Alpha blending will be turned on automatically if your texture has the alpha channel
        //{
        //    //// create object by reading an obj mesh
        //    auto sqad = Add_Obj_Mesh_Object("obj/sqad.obj");

        //    //// set object's transform
        //    Matrix4f t;
        //    t << 1, 0, 0, -0.5,
        //        0, 1, 0, 0,
        //        0, 0, 1, 1.5,
        //        0, 0, 0, 1;
        //    sqad->Set_Model_Matrix(t);

        //    //// bind texture to object
        //    sqad->Add_Texture("tex_color", OpenGLTextureLibrary::Get_Texture("window_color"));

        //    //// bind shader to object
        //    sqad->Add_Shader_Program(OpenGLShaderLibrary::Get_Shader("blend"));
        //}

        //// Here we show an example of adding a billboard particle with a star shape using alpha blending
        //// The billboard is rendered with its texture and is always facing the camera.
        //// This example will be useful if you plan to implement a CPU-based particle system.
        //{
        //    //// create object by reading an obj mesh
        //    auto sqad = Add_Obj_Mesh_Object("obj/sqad.obj");

        //    //// set object's transform
        //    Matrix4f t;
        //    t << 1, 0, 0, 0,
        //         0, 1, 0, 0,
        //         0, 0, 1, 2.5,
        //         0, 0, 0, 1;
        //    sqad->Set_Model_Matrix(t);

        //    //// bind texture to object
        //    sqad->Add_Texture("tex_color", OpenGLTextureLibrary::Get_Texture("hair"));

        //    //// bind shader to object
        //    sqad->Add_Shader_Program(OpenGLShaderLibrary::Get_Shader("phong"));
        //}

        //// Here we show an example of shading (ray-tracing) a sphere with environment mapping
        /*
        {
            //// create object by reading an obj mesh
            auto sphere2 = Add_Obj_Mesh_Object("obj/sphere.obj");

            //// set object's transform
            Matrix4f t;
            t << .6, 0, 0, 0,
                0, .6, 0, -.5,
                0, 0, .6, 1,
                0, 0, 0, 1;
            sphere2->Set_Model_Matrix(t);

            //// bind shader to object
            sphere2->Add_Shader_Program(OpenGLShaderLibrary::Get_Shader("environment")); // bind shader to object
        }
        */

        //// This for-loop updates the rendering model for each object on the list
        for (auto &mesh_obj : mesh_object_array){
            Set_Polygon_Mode(mesh_obj, PolygonMode::Fill);
            Set_Shading_Mode(mesh_obj, ShadingMode::TexAlpha);
            mesh_obj->Set_Data_Refreshed();
            mesh_obj->Initialize();
        }

        Toggle_Play();
    }
    void create_grass_bundle(float x,float y) {
        const int num_grass_blades = 4;
        const float angle_offset = 45.0f; 

        for (int i = 0; i < num_grass_blades; ++i) {
            auto grass = Add_Obj_Mesh_Object("obj/sqad.obj");
            Matrix4f t;
            t << 0.5, 0, 0, x,
                0, 0.5, 0, -0.2,
                0, 0, 1, y,
                0, 0, 0, 1;


            Eigen::AngleAxisf rotation(i * angle_offset * 3.1415926 / 180.0f, Eigen::Vector3f::UnitY());
            t.block<3, 3>(0, 0) = rotation.toRotationMatrix();

            grass->Set_Model_Matrix(t);
            grass->Set_Ka(Vector3f(x, y, 0.));
            grass->Add_Texture("tex_color", OpenGLTextureLibrary::Get_Texture("grass"));
            grass->Add_Shader_Program(OpenGLShaderLibrary::Get_Shader("billboard"));
            grass->setTime(startTime);
        }
    }

    OpenGLTriangleMesh *Add_Obj_Mesh_Object(std::string obj_file_name)
    {
        auto mesh_obj = Add_Interactive_Object<OpenGLTriangleMesh>();
        Array<std::shared_ptr<TriangleMesh<3>>> meshes;
        // Obj::Read_From_Obj_File(obj_file_name, meshes);
        Obj::Read_From_Obj_File_Discrete_Triangles(obj_file_name, meshes);

        mesh_obj->mesh = *meshes[0];
        std::cout << "load tri_mesh from obj file, #vtx: " << mesh_obj->mesh.Vertices().size() << ", #ele: " << mesh_obj->mesh.Elements().size() << std::endl;

        mesh_object_array.push_back(mesh_obj);
        return mesh_obj;
    }

    //// Go to next frame
    virtual void Toggle_Next_Frame()
    {
        //Uniform_Update();
        for (auto &mesh_obj : mesh_object_array)
            mesh_obj->setTime(GLfloat(clock() - startTime) / CLOCKS_PER_SEC);

        if (bgEffect){
            bgEffect->setResolution((float)Win_Width(), (float)Win_Height());
            bgEffect->setTime(GLfloat(clock() - startTime) / CLOCKS_PER_SEC);
            bgEffect->setFrame(frame++);
        }
        OpenGLViewer::Toggle_Next_Frame();
    }

    virtual void Run()
    {
        OpenGLViewer::Run();
    }
};

int main(int argc, char *argv[])
{
    MyDriver driver;
    driver.Initialize();
    driver.Run();
}

#endif