#pragma once

#include <algorithm>
#include <iostream>
#include <string>

#include <ft2build.h>  // NOLINT include order required by freetype
#include FT_FREETYPE_H

#pragma warning(push)
#pragma warning(disable: 4127)
#include <QApplication>
#include <QBoxLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QMainWindow>
#include <QResizeEvent>
#include <QValidator>
#pragma warning(pop)

#include <glm/glm.hpp>

#include <glbinding/gl/gl.h>
#include <glbinding/ContextInfo.h>
#include <glbinding/Version.h>

#include <globjects/base/File.h>
#include <globjects/globjects.h>
#include <globjects/logging.h>

#include <globjects/Buffer.h>
#include <globjects/Program.h>
#include <globjects/Shader.h>
#include <globjects/Texture.h>
#include <globjects/VertexArray.h>
#include <globjects/VertexAttributeBinding.h>

#include <llassetgen/llassetgen.h>

#include "WindowQt.h"

using namespace gl;


/*Taken from cginternals/globjects,
 * example: qtexample, texture
 * edited
 */

class Window : public WindowQt {
   public:
    Window(QSurfaceFormat& format) : WindowQt(format) {
        m_samplerIndex = 0;
        m_texture = nullptr;
        m_backgroundColor = glm::vec4(1.f, 1.f, 1.f, 1.f);
    }

    virtual ~Window() {
        m_texture->unref();
    }

    virtual void initializeGL() override {
        globjects::init();

        std::cout << std::endl
                  << "OpenGL Version:  " << glbinding::ContextInfo::version() << std::endl
                  << "OpenGL Vendor:   " << glbinding::ContextInfo::vendor() << std::endl
                  << "OpenGL Renderer: " << glbinding::ContextInfo::renderer() << std::endl
                  << std::endl;

        globjects::DebugMessage::enable();

#ifdef __APPLE__
        Shader::clearGlobalReplacements();
        Shader::globalReplace("#version 140", "#version 150");

        debug() << "Using global OS X shader replacement '#version 140' -> '#version 150'" << std::endl;
#endif

        // get glyph atlas

        // TODO load using own png loader instead of Qt (blocked: wait for this feature to be merged into master, then
        // pull)  loading from relative path is different here because of Qt. TODO: make it consistent, preferably for all
        // OS
        auto path = QApplication::applicationDirPath();
        auto* image = new QImage(path + "/../../data/llassetgen-rendering/testfontatlas_rgb.png");

        // mirrored: Qt flips images after
        // loading; meant as convenience, but
        // we need it to flip back here.
        auto imageFormatted = image->convertToFormat(QImage::Format_RGBA8888).mirrored(false, true);  
        auto imageData = imageFormatted.bits();

        m_texture = globjects::Texture::createDefault(GL_TEXTURE_2D);
        m_texture->ref();
        m_texture->image2D(0, GL_RGBA8, image->width(), image->height(), 0, GL_BGRA, GL_UNSIGNED_BYTE, imageData);
        // Willy told me that green and blue channels are swapped, that's why GL_BGRA is used here; we also might ignore
        // this, since we use black&white image data here?

        m_cornerBuffer = new globjects::Buffer();
        m_program = new globjects::Program();
        m_vao = new globjects::VertexArray();

        // TODO: make sure that the relative path works on other OS, too
        // openll-asset-generator/data/llassetgen-rendering
        const std::string dataPath = "./data/llassetgen-rendering";
        m_program->attach(globjects::Shader::fromFile(GL_VERTEX_SHADER, dataPath + "/shader.vert"),
                          globjects::Shader::fromFile(GL_FRAGMENT_SHADER, dataPath + "/shader.frag"));

        m_cornerBuffer->setData(
            std::array<glm::vec2, 4>{{glm::vec2(0, 0), glm::vec2(1, 0), glm::vec2(0, 1), glm::vec2(1, 1)}},
            GL_STATIC_DRAW);

        m_vao->binding(0)->setAttribute(0);
        m_vao->binding(0)->setBuffer(m_cornerBuffer, 0, sizeof(glm::vec2));
        m_vao->binding(0)->setFormat(2, GL_FLOAT, GL_FALSE, 0);

        m_program->setUniform("glyphs", m_samplerIndex);

        m_vao->enable(0);
    }

    virtual void resizeGL(QResizeEvent* event) override {
        glViewport(0, 0, event->size().width(), event->size().height());
    }

    virtual void paintGL() override {

        glClearColor(m_backgroundColor.r, m_backgroundColor.g, m_backgroundColor.b, m_backgroundColor.a);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        if (m_texture) {
            m_texture->bindActive(m_samplerIndex);
        }

        m_program->use();
        m_vao->drawArrays(GL_TRIANGLE_STRIP, 0, 4);
        m_program->release();

        if (m_texture) {
            m_texture->unbindActive(m_samplerIndex);
        }

        glDisable(GL_BLEND);
    }

    virtual void keyPressEvent(QKeyEvent* event) override {
        makeCurrent();

        switch (event->key()) {
            case Qt::Key_F5:
                globjects::File::reloadAll();
                break;
            default:
                break;
        }
        doneCurrent();
    }

   public slots:
    virtual void backgroundColorRChanged(QString value)
    {
        int red = value.toInt(); 
        m_backgroundColor.r = red / 255.f;
        paint();
    }

    virtual void backgroundColorGChanged(QString value)
    {
        int green = value.toInt();
        m_backgroundColor.g = green / 255.f;
        paint();
    }

    virtual void backgroundColorBChanged(QString value)
    {
        int blue = value.toInt();
        m_backgroundColor.b = blue / 255.f;
        paint();
    }

   protected:
    globjects::ref_ptr<globjects::Buffer> m_cornerBuffer;
    globjects::ref_ptr<globjects::Program> m_program;
    globjects::ref_ptr<globjects::VertexArray> m_vao;

    globjects::Texture* m_texture;

    glm::vec4 m_backgroundColor;
    int m_samplerIndex;
};

int main(int argc, char** argv) {
    llassetgen::init();

    std::unique_ptr<llassetgen::DistanceTransform> dt(new llassetgen::DeadReckoning());

    // TODO: don't export, but use as texture directly
    dt->importPng("./data/llassetgen-rendering/testfontatlas.png");
    dt->transform();
    dt->exportPng("./data/llassetgen-rendering/testfontatlasDT.png", -20, 50, 8);
    // TODO: exported png is corrupted, wait for master merge?

    QApplication app(argc, argv);

    // from globjects
    QSurfaceFormat format;
#ifdef __APPLE__
    format.setVersion(4, 1);  // ToDo: which version is supported on macOS?
    format.setProfile(QSurfaceFormat::CoreProfile);
#else
    format.setVersion(4, 3);
#endif
    format.setDepthBufferSize(16);

    Window* glwindow = new Window(format);



    // GUI

    QMainWindow window;
    window.setMinimumSize(640, 480);
    window.setWindowTitle("Open Font Asset Generator");

    auto guiGroupBox = new QGroupBox("Parameters");
    auto *guiLayout = new QBoxLayout(QBoxLayout::LeftToRight);

    auto colorValidator = new QIntValidator(0, 255);

    //Background Color RED
    auto *backgroundR = new QLineEdit();
    auto *labelR = new QLabel("Background Red:");
    labelR->setMaximumWidth(90);
    backgroundR->setValidator(colorValidator);
    backgroundR->setPlaceholderText("255");
    backgroundR->setMaximumWidth(45);

    QObject::connect(backgroundR, SIGNAL(textEdited(QString)), glwindow, SLOT(backgroundColorRChanged(QString)));
    
    guiLayout->addWidget(labelR);
    guiLayout->addWidget(backgroundR, 0, Qt::AlignLeft);
    guiGroupBox->setLayout(guiLayout);


    //Background Color GREEN
    auto *backgroundG = new QLineEdit();
    auto *labelG = new QLabel("Background Green:");
    labelG->setMaximumWidth(90);
    backgroundG->setValidator(colorValidator);
    backgroundG->setPlaceholderText("255");
    backgroundG->setMaximumWidth(45);

    QObject::connect(backgroundG, SIGNAL(textEdited(QString)), glwindow, SLOT(backgroundColorGChanged(QString)));

    guiLayout->addWidget(labelG);
    guiLayout->addWidget(backgroundG, 0, Qt::AlignLeft);
    guiGroupBox->setLayout(guiLayout);


    //Background Color BLUE
    auto *backgroundB = new QLineEdit();
    auto *labelB = new QLabel("Background Blue:");
    labelB->setMaximumWidth(90);
    backgroundB->setValidator(colorValidator);
    backgroundB->setPlaceholderText("255");
    backgroundB->setMaximumWidth(45);

    QObject::connect(backgroundB, SIGNAL(textEdited(QString)), glwindow, SLOT(backgroundColorBChanged(QString)));

    guiLayout->addWidget(labelB);
    guiLayout->addWidget(backgroundB, 0, Qt::AlignLeft);
    guiGroupBox->setLayout(guiLayout);


    auto *mainLayout = new QBoxLayout(QBoxLayout::TopToBottom);
    mainLayout->addWidget(guiGroupBox);
    mainLayout->addWidget(QWidget::createWindowContainer(glwindow));

    // since window already has a special layout, we have to put our layout on a widget and then set it as central widget
    auto central = new QWidget();
    central->setLayout(mainLayout);
    
    window.setCentralWidget(central);

    window.show();

    return app.exec();
}
