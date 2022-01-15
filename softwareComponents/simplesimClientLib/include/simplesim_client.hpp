#pragma once

#include <map>
#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_set>
#include <utility>
#include <vector>

#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkInteractorStyle.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkNew.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>

#include "atoms/guarded.hpp"
#include "configuration/rofibot.hpp"
#include "configuration/serialization.hpp"
#include "legacy/configuration/IO.h"

#include <QMainWindow>
#include <QTimer>
#include <QTreeWidgetItem>

#define vtkTypeMacro_( thisClass, superClass ) \
    vtkTypeMacro( thisClass, superClass ) static_assert( true, "require semicolon" )

namespace Ui {
    class SimplesimClient;
}

namespace rofi::simplesim
{
namespace detail
{
    class ModuleRenderInfo
    {
    public:
        std::vector< vtkSmartPointer< vtkActor > > componentActors;
        std::unordered_set< int > activeConnectors;
    };

} // namespace detail

constexpr int simSpeed = 10;
// TODO use the same mapper for all modules
// TODO use the same property for setting the modules
class SimplesimClient : public QMainWindow
{
    Q_OBJECT

private:
    class UpdateConfigurationCommand : public vtkCommand
    {
        vtkTypeMacro_( UpdateConfigurationCommand, vtkCommand );

    public:
        static UpdateConfigurationCommand * New()
        {
            return new UpdateConfigurationCommand;
        }

        void Execute( vtkObject * /* caller */, unsigned long /* eventId */, void * /* callData */ )
        {
            assert( client );

            client->renderCurrentConfiguration();
        }

        SimplesimClient * client;
    };

public:
    SimplesimClient();

    ~SimplesimClient();

    // Blocks until the user closes the window
    void run()
    {
        assert( _renderWindow.Get() != nullptr );
        assert( _renderWindowInteractor.Get() != nullptr );

        renderCurrentConfiguration();

        //_renderWindowInteractor->Start();
    }

    // Can be called from any thread
    void onConfigurationUpdate(
            std::shared_ptr< const rofi::configuration::Rofibot > newConfiguration )
    {
        assert( newConfiguration );
        assert( newConfiguration->isValid( rofi::configuration::SimpleColision() ).first );
        _currentConfiguration.visit( [ &newConfiguration ]( auto & currConfig ) {
            currConfig = std::move( newConfiguration );
        } );
    }

protected:
    void timerEvent( QTimerEvent *event );

private slots:

    void pauseButton();

    void speedChanged( int speed );

private:
    Ui::SimplesimClient* ui;

    QTreeWidgetItem* configToQItem( const rofi::configuration::Rofibot& rofibot );

    std::shared_ptr< const rofi::configuration::Rofibot > getCurrentConfig() const
    {
        return _currentConfiguration.visit( [ this ]( const auto & config ) { return config; } );
    }

    void clearRenderer();
    void renderCurrentConfiguration();


    atoms::Guarded< std::shared_ptr< const rofi::configuration::Rofibot > > _currentConfiguration;
    std::shared_ptr< const rofi::configuration::Rofibot > _lastRenderedConfiguration;

    vtkNew< vtkRenderer > _renderer;
    vtkNew< vtkRenderWindow > _renderWindow;
    vtkNew< vtkInteractorStyleTrackballCamera > _interactorStyle;
    //vtkNew< UpdateConfigurationCommand > _updateConfigurationCommand;
    vtkNew< vtkRenderWindowInteractor > _renderWindowInteractor;

    int _timer;

    bool paused = false;

    std::map< rofi::configuration::ModuleId, detail::ModuleRenderInfo > _moduleRenderInfos;
};

} // namespace rofi::simplesim
