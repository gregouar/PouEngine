
#ifndef   POUENGINE_TYPES
#define   POUENGINE_TYPES

#include <iostream>
#include <chrono>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


namespace pou
{

class SceneObject;
class SceneNode;
/**class SceneEntity;
class Light;
class ShadowCaster;**/

template<class AssetType> class AssetHandler;
class TextureAsset;
class SpriteSheetAsset;
class MaterialAsset;
class MeshAsset;
class SkeletonModelAsset;
//class CharacterModelAsset;

class Timer;

enum RendererName
{
    Renderer_Default,
    Renderer_Scene,
    Renderer_Instancing,
};

enum RenderereOrder
{
    Renderer_First,
    Renderer_Middle,
    Renderer_Last,
    Renderer_Unique,
};

enum BlendMode
{
    BlendMode_None,
    BlendMode_Add,
    BlendMode_Alpha,
    BlendMode_Accu,
    BlendMode_Custom,
};

enum AssetLoadType
{
    LoadType_Now = 0,
    LoadType_InThread = 1,
};

enum AssetLoadSource
{
    LoadSource_None,
    LoadSource_File,
    LoadSource_Memory,
    LoadSource_Stream,
};

enum NotificationType
{
    Notification_SenderDestroyed,
    Notification_AssetLoaded,
    Notification_NodeMoved,
    Notification_UpdateCmb,
    Notification_ModelChanged,
    Notification_TextureChanged,
    Notification_TextureIsAboutToChange,
    Notification_UpdateShadow,
   // Notification_LightMoved,
};

enum LightType
{
    LightType_Omni,
    LightType_Directional,
    LightType_Spot,
};

//Could use flag
enum ShadowCastingType
{
    ShadowCasting_None,
    ShadowCasting_OnlyDirectional,
    ShadowCasting_OnlyDynamic,
    ShadowCasting_All,
};

enum ShadowVolumeType
{
    OneSidedShadow,
    TwoSidedShadow,
    MirroredTwoSidedShadow,
};


enum SkelAnimCmdType
{
    Unknown_Command,
    Move_To,
    Rotate_To,
    Scale_To,
};

enum CommandPoolName
{
    COMMANDPOOL_DEFAULT,
    COMMANDPOOL_SHORTLIVED, //fort short-lived command buffers
    COMMANDPOOL_TEXTURESLOADING, //for texture loading thread
    COMMANDPOOL_MESHESLOADING, //for meshes loading thread
    COMMANDPOOL_NBR_NAMES,
};

typedef std::chrono::duration<double, std::chrono::seconds::period> Time;

typedef unsigned int AssetTypeId;
typedef unsigned int NodeTypeId;
typedef unsigned int ObjectTypeId;

typedef glm::vec4 Color;

typedef AssetHandler<TextureAsset>      TexturesHandler;
typedef AssetHandler<SpriteSheetAsset>  SpriteSheetsHandler;
typedef AssetHandler<MaterialAsset>     MaterialsHandler;
typedef AssetHandler<SkeletonModelAsset>     SkeletonModelsHandler;
//typedef AssetHandler<CharacterModelAsset>     CharacterModelsHandler;
//typedef AssetHandler<MeshAsset>     MeshesHandler;

const std::string emptyString;



}

#endif
