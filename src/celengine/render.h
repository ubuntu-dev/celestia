// render.h
//
// Copyright (C) 2001-2007, Celestia Development Team
// Contact: Chris Laurel <claurel@gmail.com>
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

#ifndef _CELENGINE_RENDER_H_
#define _CELENGINE_RENDER_H_

#include <vector>
#include <list>
#include <string>
#include <celmath/frustum.h>
#include <celengine/observer.h>
#include <celengine/universe.h>
#include <celengine/selection.h>
#include <celengine/glcontext.h>
#include <celengine/starcolors.h>
#include <celengine/rendcontext.h>
#include <celtxf/texturefont.h>


class RendererWatcher;
class FrameTree;

struct LightSource
{
    Point3d position;
    Color color;
    float luminosity;
    float radius;
};


struct RenderListEntry
{
    enum RenderableType
    {
        RenderableStar,
        RenderableBody,
        RenderableCometTail,
        RenderableBodyAxes,
        RenderableFrameAxes,
        RenderableSunDirection,
        RenderableVelocityVector,
    };

    const Star* star;
    Body* body;
    Point3f position;
    Vec3f sun;
    float distance;
    float radius;
    float centerZ;
    float nearZ;
    float farZ;
    float discSizeInPixels;
    float appMag;
    RenderableType renderableType;
    bool isOpaque;
    std::vector<LightSource>* lightSourceList;
};


class Renderer
{
 public:
    Renderer();
    ~Renderer();

    struct DetailOptions
    {
        DetailOptions();
        unsigned int ringSystemSections;
        unsigned int orbitPathSamplePoints;
        unsigned int shadowTextureSize;
        unsigned int eclipseTextureSize;
    };

    bool init(GLContext*, int, int, DetailOptions&);
    void shutdown() {};
    void resize(int, int);

    float calcPixelSize(float fov, float windowHeight);
    void setFaintestAM45deg(float);
    float getFaintestAM45deg();

    void setRenderMode(int);
    void autoMag(float& faintestMag);
    void render(const Observer&,
                const Universe&,
                float faintestVisible,
                const Selection& sel);
    
    enum {
        NoLabels            = 0x000,
        StarLabels          = 0x001,
        PlanetLabels        = 0x002,
        MoonLabels          = 0x004,
        ConstellationLabels = 0x008,
        GalaxyLabels        = 0x010,
        AsteroidLabels      = 0x020,
        SpacecraftLabels    = 0x040,
        LocationLabels      = 0x080,
        CometLabels         = 0x100,
        NebulaLabels        = 0x200,
        OpenClusterLabels   = 0x400,
        I18nConstellationLabels = 0x800,
        BodyLabelMask       = (PlanetLabels | MoonLabels | AsteroidLabels | SpacecraftLabels | CometLabels),
    };

    enum {
        ShowNothing         = 0x0000,
        ShowStars           = 0x0001,
        ShowPlanets         = 0x0002,
        ShowGalaxies        = 0x0004,
        ShowDiagrams        = 0x0008,
        ShowCloudMaps       = 0x0010,
        ShowOrbits          = 0x0020,
        ShowCelestialSphere = 0x0040,
        ShowNightMaps       = 0x0080,
        ShowAtmospheres     = 0x0100,
        ShowSmoothLines     = 0x0200,
        ShowEclipseShadows  = 0x0400,
        ShowStarsAsPoints   = 0x0800,
        ShowRingShadows     = 0x1000,
        ShowBoundaries      = 0x2000,
        ShowAutoMag         = 0x4000,
        ShowCometTails      = 0x8000,
        ShowMarkers         = 0x10000,
        ShowPartialTrajectories = 0x20000,
        ShowNebulae         = 0x40000,
        ShowOpenClusters    = 0x80000,
        ShowCloudShadows    = 0x200000,
    };

    enum StarStyle 
    {
        FuzzyPointStars  = 0,
        PointStars       = 1,
        ScaledDiscStars  = 2,
        StarStyleCount   = 3,
    };

    // constants
    static const int DefaultRenderFlags = Renderer::ShowStars          |
                                          Renderer::ShowPlanets        |
                                          Renderer::ShowGalaxies       |
                                          Renderer::ShowCloudMaps      |
                                          Renderer::ShowAtmospheres    |
                                          Renderer::ShowEclipseShadows |
                                          Renderer::ShowRingShadows    |
                                          Renderer::ShowCometTails     |
                                          Renderer::ShowNebulae        |
                                          Renderer::ShowOpenClusters   |
                                          Renderer::ShowAutoMag        |
                                          Renderer::ShowSmoothLines;

    int getRenderFlags() const;
    void setRenderFlags(int);
    int getLabelMode() const;
    void setLabelMode(int);
    float getAmbientLightLevel() const;
    void setAmbientLightLevel(float);
    float getMinimumOrbitSize() const;
    void setMinimumOrbitSize(float);
    float getMinimumFeatureSize() const;
    void setMinimumFeatureSize(float);
    float getDistanceLimit() const;
    void setDistanceLimit(float);
    int getOrbitMask() const;
    void setOrbitMask(int);
    int getScreenDpi() const;
    void setScreenDpi(int);
    const ColorTemperatureTable* getStarColorTable() const;
    void setStarColorTable(const ColorTemperatureTable*);
    bool getVideoSync() const;
    void setVideoSync(bool);

    bool getFragmentShaderEnabled() const;
    void setFragmentShaderEnabled(bool);
    bool fragmentShaderSupported() const;
    bool getVertexShaderEnabled() const;
    void setVertexShaderEnabled(bool);
    bool vertexShaderSupported() const;

    GLContext* getGLContext() { return context; }

    void setStarStyle(StarStyle);
    StarStyle getStarStyle() const;
    void setResolution(unsigned int resolution);
    unsigned int getResolution();

    void loadTextures(Body*);

    // Label related methods
    static const int MaxLabelLength = 32;
    struct Annotation
    {
        char labelText[MaxLabelLength];
        const Marker* marker;
        Color color;
        Point3f position;

        bool operator<(const Annotation&) const;
    };
        
    void addForegroundAnnotation(const Marker*, const char* labelText, Color, const Point3f&, float depth = -1);
    void addBackgroundAnnotation(const Marker*, const char* labelText, Color, const Point3f&, float depth = -1);
    void addBackgroundAnnotation(const std::string&, Color, const Point3f&, float depth = -1);
    void addSortedAnnotation(const Marker* marker, const std::string&, Color, const Point3f&);
    
    void clearAnnotations(std::vector<Annotation>&);
	void clearSortedAnnotations();

    void invalidateOrbitCache();
    
    struct OrbitPathListEntry
    {
        float centerZ;
        float radius;
        Body* body;
        const Star* star;
        Point3f origin;
        float opacity;

        bool operator<(const OrbitPathListEntry&) const;
    };        

    enum FontStyle
    {
        FontNormal = 0,
        FontLarge  = 1,
        FontCount  = 2,
    };
    
    void setFont(FontStyle, TextureFont*);
    TextureFont* getFont(FontStyle) const;

    bool settingsHaveChanged() const;
    void markSettingsChanged();

    void addWatcher(RendererWatcher*);
    void removeWatcher(RendererWatcher*);
    void notifyWatchers() const;

 public:
    // Internal types
    // TODO: Figure out how to make these private.  Even with a friend
    // 
    struct Particle
    {
        Point3f center;
        float size;
        Color color;
        float pad0, pad1, pad2;
    };

    struct RenderProperties
    {
        RenderProperties() :
            surface(NULL),
            atmosphere(NULL),
            rings(NULL),
            radius(1.0f),
            semiAxes(1.0f, 1.0f, 1.0f),
            model(InvalidResource),
            orientation(1.0f),
            locations(NULL)
        {};

        Surface* surface;
        const Atmosphere* atmosphere;
        RingSystem* rings;
        float radius;
        Vec3f semiAxes;
        ResourceHandle model;
        Quatf orientation;
        std::vector<EclipseShadow>* eclipseShadows;
        std::vector<Location*>* locations;
    };

    class StarVertexBuffer
    {
    public:
        StarVertexBuffer(unsigned int _capacity);
        ~StarVertexBuffer();
        void start();
        void render();
        void finish();
        void addStar(const Point3f&, const Color&, float);
        void setBillboardOrientation(const Quatf&);

    private:
        unsigned int capacity;
        unsigned int nStars;
        float* vertices;
        float* texCoords;
        unsigned char* colors;
        Vec3f v0, v1, v2, v3;
    };


    class PointStarVertexBuffer
    {
    public:
        PointStarVertexBuffer(unsigned int _capacity);
        ~PointStarVertexBuffer();
        void startPoints(const GLContext&);
        void startSprites(const GLContext&);
        void render();
        void finish();
        void addStar(const Point3f& f, const Color&, float);
		void setTexture(Texture*);

    private:
        struct StarVertex
        {
            Point3f position;
            float size;
            unsigned char color[4];
            float pad;
        };

        unsigned int capacity;
        unsigned int nStars;
        StarVertex* vertices;
        const GLContext* context;
        bool useSprites;
		Texture* texture;
    };

 private:
    struct SkyVertex
    {
        float x, y, z;
        unsigned char color[4];
    };

    struct SkyContourPoint
    {
        Vec3f v;
        Vec3f eyeDir;
        float centerDist;
        float eyeDist;
        float cosSkyCapAltitude;
    };

    template <class OBJ> struct ObjectLabel
    {
        OBJ*        obj;
        std::string label;

        ObjectLabel() :
            obj  (NULL),
            label("")
        {};

        ObjectLabel(OBJ* _obj, const std::string& _label) :
            obj  (_obj),
            label(_label)
        {};

        ObjectLabel(const ObjectLabel& objLbl) :
            obj  (objLbl.obj),
            label(objLbl.label)
        {};

        ObjectLabel& operator = (const ObjectLabel& objLbl)
        {
            obj   = objLbl.obj;
            label = objLbl.label;
            return *this;
        };
    };

    enum LabelAlignment
    {
        AlignCenter,
        AlignLeft,
        AlignRight
    };
    typedef ObjectLabel<Star>          StarLabel;
    typedef ObjectLabel<DeepSkyObject> DSOLabel;    // currently not used
    
    struct DepthBufferPartition
    {
        int index;
        float nearZ;
        float farZ;
    };

 private:
    void setFieldOfView(float);
    void renderStars(const StarDatabase& starDB,
                     float faintestVisible,
                     const Observer& observer);
    void renderPointStars(const StarDatabase& starDB,
                          float faintestVisible,
                          const Observer& observer);
    void renderDeepSkyObjects(const Universe&,
                              const Observer&,
                              float faintestMagNight);
    void renderCelestialSphere(const Observer& observer);
    void buildRenderLists(const Point3d& astrocentricObserverPos,
                          const Frustum& viewFrustum,
                          const FrameTree* tree,
                          const Observer& observer,
                          double now,
                          std::vector<LightSource>* lightSourceList);
    void addStarOrbitToRenderList(const Star& star,
                                  const Observer& observer,
                                  double now);

    void renderObject(Point3f pos,
                      float distance,
                      double now,
                      Quatf cameraOrientation,
                      float nearPlaneDistance,
                      float farPlaneDistance,
                      RenderProperties& obj,
                      const LightingState&);

    void renderPlanet(Body& body,
                      Point3f pos,
                      float distance,
                      float appMag,
                      const Observer& observer,
                      const Quatf& cameraOrientation,
                      const vector<LightSource>& lightSources,
                      float, float);

    void renderStar(const Star& star,
                    Point3f pos,
                    float distance,
                    float appMag,
                    Quatf orientation,
                    double now,
                    float, float);

    void renderAxes(Body& body,
                    Point3f pos,
                    float distance,
                    double now,
                    float nearPlaneDistance,
                    float farPlaneDistance,
                    RenderListEntry::RenderableType renderableType);

    void renderSunDirection(Body& body,
                            Point3f pos,
                            float distance,
                            double now,
                            const vector<LightSource>& lightSources,
                            float nearPlaneDistance,
                            float farPlaneDistance);

    void renderVelocityVector(Body& body,
                              Point3f pos,
                              float distance,
                              double now,
                              float nearPlaneDistance,
                              float farPlaneDistance);

    void renderCometTail(const Body& body,
                         Point3f pos,
                         double now,
                         const vector<LightSource>& lightSources,
                         float discSizeInPixels);

    void renderBodyAsParticle(Point3f center,
                              float appMag,
                              float _faintestMag,
                              float discSizeInPixels,
                              Color color,
                              const Quatf& cameraOrientation,
                              float renderDistance,
                              bool useHalos);
    void renderObjectAsPoint(Point3f center,
                             float radius,
                             float appMag,
                             float _faintestMag,
                             float discSizeInPixels,
                             Color color,
                             const Quatf& cameraOrientation,
                             bool useHalos,
                             bool emissive);

    void renderEllipsoidAtmosphere(const Atmosphere& atmosphere,
                                   Point3f center,
                                   const Quatf& orientation,
                                   Vec3f semiAxes,
                                   const Vec3f& sunDirection,
                                   Color ambientColor,
                                   float fade,
                                   bool lit);

    void renderLocations(const vector<Location*>& locations,
                         const Quatf& cameraOrientation,
                         const Point3d& bodyPosition,
                         const Quatd& bodyOrientation,
                         float scale);
                   
    // Render an item from the render list                   
    void renderItem(const RenderListEntry& rle,
                    const Observer& observer,
                    const Quatf& cameraOrientation,
                    float nearPlaneDistance,
                    float farPlaneDistance);

    bool testEclipse(const Body& receiver,
                     const Body& caster,
                     const DirectionalLight& light,
                     double now,
                     vector<EclipseShadow>& shadows);

    void labelConstellations(const AsterismList& asterisms,
                             const Observer& observer);
    void renderParticles(const std::vector<Particle>& particles,
                         Quatf orientation);
    
    
    void addAnnotation(std::vector<Annotation>&,
                       const Marker*, const char* labelText, Color, const Point3f&, float depth = -1);
    void renderAnnotations(const std::vector<Annotation>&, FontStyle fs, LabelAlignment la);
    void renderBackgroundAnnotations(FontStyle fs, LabelAlignment la);
    void renderForegroundAnnotations(FontStyle fs, LabelAlignment la);
    std::vector<Annotation>::iterator renderSortedAnnotations(std::vector<Annotation>::iterator,
                                                              float nearDist,
                                                              float farDist,
                                                              FontStyle fs);
    void renderMarkers(const MarkerList&,
                       const UniversalCoord& position,
                       double jd);

    void renderOrbit(const OrbitPathListEntry&,
                     double now,
                     const Quatf& cameraOrientation,
                     const Frustum& frustum,
                     float nearDist,
                     float farDist);

 private:
    GLContext* context;

    int windowWidth;
    int windowHeight;
    float fov;
    int screenDpi;
    float corrFac;
    float pixelSize;
    float faintestAutoMag45deg;
    TextureFont* font[FontCount];

    int renderMode;
    int labelMode;
    int renderFlags;
    int orbitMask;
    float ambientLightLevel;
    bool fragmentShaderEnabled;
    bool vertexShaderEnabled;
    float brightnessBias;

    float brightnessScale;
    float faintestMag;
    float faintestPlanetMag;
    float saturationMagNight;
    float saturationMag;
    StarStyle starStyle;

    Color ambientColor;
    std::string displayedSurface;

    StarVertexBuffer* starVertexBuffer;
    PointStarVertexBuffer* pointStarVertexBuffer;
	PointStarVertexBuffer* glareVertexBuffer;
    std::vector<RenderListEntry> renderList;
    std::vector<DepthBufferPartition> depthPartitions;
    std::vector<Particle> glareParticles;
    std::vector<Annotation> backgroundAnnotations;
    std::vector<Annotation> foregroundAnnotations;
    std::vector<Annotation> depthSortedAnnotations;
    std::vector<OrbitPathListEntry> orbitPathList;
    std::vector<EclipseShadow> eclipseShadows[MaxLights];
    std::vector<const Star*> nearStars;

    std::list<std::vector<LightSource>* > lightSourceLists;

    double modelMatrix[16];
    double projMatrix[16];

    bool useCompressedTextures;
    bool useVertexPrograms;
    bool useRescaleNormal;
    bool usePointSprite;
    bool useClampToBorder;
    unsigned int textureResolution;

    DetailOptions detailOptions;
    
    bool useNewStarRendering;

    uint32 frameCount;

 public:
    struct OrbitSample 
    {
        double t;
        Point3d pos;
        
        OrbitSample(const Point3d& _pos, double _t) : t(_t), pos(_pos) { }
        OrbitSample() { }
    };

    struct OrbitSection
    {
        Capsuled boundingVolume;
        uint32 firstSample;
    };
    
    struct CachedOrbit
    {
        std::vector<OrbitSample> trajectory;
        std::vector<OrbitSection> sections;
        uint32 lastUsed;
    };

 private:
    typedef std::map<const Orbit*, CachedOrbit*> OrbitCache;
    OrbitCache orbitCache;
    uint32 lastOrbitCacheFlush;

    float minOrbitSize;
    float distanceLimit;
    float minFeatureSize;
    uint32 locationFilter;

    SkyVertex* skyVertices;
    uint32* skyIndices;
    SkyContourPoint* skyContour;

    const ColorTemperatureTable* colorTemp;
    
    Selection highlightObject;

    bool videoSync;
    bool settingsChanged;

    std::list<RendererWatcher*> watchers;

 public:
    // Colors for all lines and labels
    static Color StarLabelColor;
    static Color PlanetLabelColor;
    static Color MoonLabelColor;
    static Color AsteroidLabelColor;
    static Color CometLabelColor;
    static Color SpacecraftLabelColor;
    static Color LocationLabelColor;
    static Color GalaxyLabelColor;
    static Color NebulaLabelColor;
    static Color OpenClusterLabelColor;
    static Color ConstellationLabelColor;
    static Color EquatorialGridLabelColor;

    static Color StarOrbitColor;
    static Color PlanetOrbitColor;
    static Color MoonOrbitColor;
    static Color AsteroidOrbitColor;
    static Color CometOrbitColor;
    static Color SpacecraftOrbitColor;
    static Color SelectionOrbitColor;

    static Color ConstellationColor;
    static Color BoundaryColor;
    static Color EquatorialGridColor;
};


class RendererWatcher
{
 public:
    RendererWatcher() {};
    virtual ~RendererWatcher() {};

    virtual void notifyRenderSettingsChanged(const Renderer*) = 0;
};


#endif // _CELENGINE_RENDER_H_
