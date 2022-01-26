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
#include "changecolor.hpp"

#include <QMainWindow>
#include <QTimer>
#include <QTreeWidgetItem>
#include <QWidgetItem>

namespace Ui {
    class SimplesimClient;
    class ChangeColor;
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
    public:
        vtkTypeMacro( UpdateConfigurationCommand, vtkCommand );

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

        initInfoTree( *getCurrentConfig() );

        renderCurrentConfiguration();

        //_renderWindowInteractor->Start();
    }

    // Can be called from any thread
    void onConfigurationUpdate(
            std::shared_ptr< const rofi::configuration::Rofibot > newConfiguration )
    {
        // assert( newConfiguration.isPreparedAndValid() ); // TODO
        _currentConfiguration.visit( [ &newConfiguration ]( auto & currConfig ) {
            currConfig = std::move( newConfiguration );
        } );
    }

    void timerEvent( QTimerEvent *event );

    void colorModule( rofi::configuration::ModuleId module,
                      double color[ 3 ],
                      int component = -1 );
public slots:
    void setColor( int color );

private slots:

    void itemSelected( QTreeWidgetItem* selected );

    void pauseButton();

    void changeColorWindow();
 //   void speedChanged( int speed );

private:
    Ui::SimplesimClient* ui;

    void initInfoTree( const rofi::configuration::Rofibot& rofibot );

    void updateInfoTree( const rofi::configuration::Rofibot& rofibot );

    std::shared_ptr< const rofi::configuration::Rofibot > getCurrentConfig() const
    {
        return _currentConfiguration.visit( [ this ]( const auto & config ) { return config; } );
    }

    void clearRenderer();
    void renderCurrentConfiguration();


    atoms::Guarded< std::shared_ptr< const rofi::configuration::Rofibot > > _currentConfiguration;
    std::shared_ptr< const rofi::configuration::Rofibot > _lastRenderedConfiguration;

    std::unique_ptr< ChangeColor > _changeColorWindow;
    vtkNew< vtkRenderer > _renderer;
    vtkNew< vtkRenderWindow > _renderWindow;
    vtkNew< vtkInteractorStyleTrackballCamera > _interactorStyle;
    //vtkNew< UpdateConfigurationCommand > _updateConfigurationCommand;
    vtkNew< vtkRenderWindowInteractor > _renderWindowInteractor;

    int _timer;

    bool _paused = false;

    int _lastModule = -1;
    double _lastColor[ 3 ];

    std::map< rofi::configuration::ModuleId, detail::ModuleRenderInfo > _moduleRenderInfos;
};

} // namespace rofi::simplesim
