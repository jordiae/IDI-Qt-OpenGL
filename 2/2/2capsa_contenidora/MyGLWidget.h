#define GLM_FORCE_RADIANS
#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLWidget>
#include <QOpenGLShader>
#include <QOpenGLShaderProgram>
#include <QKeyEvent>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "Model/model.h"

class MyGLWidget : public QOpenGLWidget, protected QOpenGLFunctions_3_3_Core 
{
  Q_OBJECT

  public:
    MyGLWidget (QWidget *parent=0);
    ~MyGLWidget ();

  protected:
    // initializeGL - Aqui incluim les inicialitzacions del contexte grafic.
    virtual void initializeGL ( );
    // paintGL - Mètode cridat cada cop que cal refrescar la finestra.
    // Tot el que es dibuixa es dibuixa aqui.
    virtual void paintGL ( );
    // resizeGL - És cridat quan canvia la mida del widget
    virtual void resizeGL (int width, int height);
    // keyPressEvent - Es cridat quan es prem una tecla
    virtual void keyPressEvent (QKeyEvent *event);

  private:
    void createBuffers ();
    void carregaShaders ();
    void modelTransform ();

    void modelTransformHomer();
    void modelTransformTerra();

    void projectTransform();

    void viewTransform();

    void ini_camera();

    void capsa_contenidora(glm::vec3 punt_min, glm::vec3 punt_max);


    // attribute locations
    GLuint vertexLoc, colorLoc;
    // uniform locations
    GLuint transLoc, projLoc, viewLoc;
    // VAO i VBO names
    //GLuint VAO_Casa, VBO_CasaPos, VBO_CasaCol;

    GLuint VAO_Homer, VBO_HomerPos, VBO_HomerCol;

    GLuint VAO_Terra, VBO_TerraPos, VBO_TerraCol;
    
    // Program
    QOpenGLShaderProgram *program;
    // Internal vars
    float scale;
    glm::vec3 pos;

    float rot;


    glm::vec3 OBS;
    glm::vec3 VRP;
    glm::vec3 UP;
    float FOV;
    float ra;
    float znear;
    float zfar;

    float alpha;

    Model homer;

    glm::vec3 centre_capsa_contenidora;
    float radi_esfera;
    
};

