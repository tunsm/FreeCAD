/***************************************************************************
 *   Copyright (c) 2008 Werner Mayer <wmayer[at]users.sourceforge.net>     *
 *                                                                         *
 *   This file is part of the FreeCAD CAx development system.              *
 *                                                                         *
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU Library General Public           *
 *   License as published by the Free Software Foundation; either          *
 *   version 2 of the License, or (at your option) any later version.      *
 *                                                                         *
 *   This library  is distributed in the hope that it will be useful,      *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU Library General Public License for more details.                  *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this library; see the file COPYING.LIB. If not,    *
 *   write to the Free Software Foundation, Inc., 59 Temple Place,         *
 *   Suite 330, Boston, MA  02111-1307, USA                                *
 *                                                                         *
 ***************************************************************************/


#ifndef GUI_NAVIGATIONSTYLE_H
#define GUI_NAVIGATIONSTYLE_H

#include <Inventor/C/basic.h>
#include <Inventor/SbBox2s.h>
#include <Inventor/SbVec2f.h>
#include <Inventor/SbVec2s.h>
#include <Inventor/SbVec3f.h>
#include <Inventor/SbPlane.h>
#include <Inventor/SbRotation.h>
#include <Inventor/SbTime.h>
#include <QCursor>
#include <QEvent>
#include <Base/BaseClass.h>

// forward declarations
class SoEvent;
class SoQtViewer;
class SoCamera;
class SoSensor;
class SbSphereSheetProjector;

namespace Gui {

class View3DInventorViewer;
class AbstractMouseSelection;

/**
 * @author Werner Mayer
 */
class GuiExport NavigationStyleEvent : public QEvent
{
public:
    NavigationStyleEvent(const Base::Type& s);
    ~NavigationStyleEvent();
    const Base::Type& style() const;
private:
    Base::Type t;
};

/**
 * The navigation style base class
 * @author Werner Mayer
 */
class GuiExport NavigationStyle : public Base::BaseClass
{
    TYPESYSTEM_HEADER();

public:
    enum ViewerMode {
        IDLE,
        INTERACT,
        ZOOMING,
        BOXZOOM,
        PANNING,
        DRAGGING,
        SPINNING,
        SEEK_WAIT_MODE,
        SEEK_MODE,
        SELECTION
    };

    enum SelectionMode {
        Lasso       = 0,  /**< Select objects using a lasso. */
        Rectangle   = 1,  /**< Select objects using a rectangle. */
        BoxZoom     = 2,  /**< Perform a box zoom. */
        Clip        = 3,  /**< Clip objects using a lasso. */
    };

    enum OrbitStyle {
        Turntable,
        Trackball
    };

public:
    NavigationStyle();
    virtual ~NavigationStyle();

    NavigationStyle& operator = (const NavigationStyle& ns);
    void setViewer(View3DInventorViewer*);

    void setAnimationEnabled(const SbBool enable);
    SbBool isAnimationEnabled(void) const;

    void startAnimating(const SbVec3f& axis, float velocity);
    void stopAnimating(void);
    SbBool isAnimating(void) const;

    void setZoomInverted(SbBool);
    SbBool isZoomInverted() const;

    void updateAnimation();
    void redraw();

    void setCameraOrientation(const SbRotation& rot);
    void lookAtPoint(const SbVec3f&);
    void boxZoom(const SbBox2s& box);
    virtual void viewAll();

    void setViewingMode(const ViewerMode newmode);
    int getViewingMode() const;
    virtual SbBool processEvent(const SoEvent * const ev);

    void setPopupMenuEnabled(const SbBool on);
    SbBool isPopupMenuEnabled(void) const;

    void startSelection(AbstractMouseSelection*);
    void startSelection(SelectionMode = Lasso);
    void stopSelection();
    SbBool isSelecting() const;
    const std::vector<SbVec2s>& getPolygon(SbBool* clip_inner=0) const;

    void setOrbitStyle(OrbitStyle style);
    OrbitStyle getOrbitStyle() const;

protected:
    void initialize();
    void finalize();

    void interactiveCountInc(void);
    void interactiveCountDec(void);
    int getInteractiveCount(void) const;

    SbBool isViewing(void) const;
    void setViewing(SbBool);
    SbBool isSeekMode(void) const;
    void setSeekMode(SbBool enable);
    SbBool seekToPoint(const SbVec2s screenpos);
    void seekToPoint(const SbVec3f& scenepos);
    SbBool lookAtPoint(const SbVec2s screenpos);

    void reorientCamera(SoCamera * camera, const SbRotation & rot);
    void panCamera(SoCamera * camera,
                   float vpaspect,
                   const SbPlane & panplane,
                   const SbVec2f & previous,
                   const SbVec2f & current);
    void pan(SoCamera* camera);
    void panToCenter(const SbPlane & pplane, const SbVec2f & currpos);
    void zoom(SoCamera * camera, float diffvalue);
    void zoomByCursor(const SbVec2f & thispos, const SbVec2f & prevpos);
    void spin(const SbVec2f & pointerpos);
    SbBool doSpin();

    SbBool handleEventInForeground(const SoEvent* const e);
    virtual SbBool processSoEvent(const SoEvent * const ev);
    virtual void openPopupMenu(const SbVec2s& position);

    void clearLog(void);
    void addToLog(const SbVec2s pos, const SbTime time);

protected:
    struct { // tracking mouse movement in a log
        short size;
        short historysize;
        SbVec2s * position;
        SbTime * time;
    } log;

    View3DInventorViewer* viewer;
    ViewerMode currentmode;
    SbVec2f lastmouseposition;
    SbPlane panningplane;
    SbTime prevRedrawTime;
    SbTime centerTime;
    SbBool lockrecenter;
    SbBool menuenabled;
    SbBool ctrldown, shiftdown, altdown;
    SbBool button1down, button2down, button3down;
    SbBool invertZoom;

    /** @name Mouse model */
    //@{
    AbstractMouseSelection* mouseSelection;
    std::vector<SbVec2s> pcPolygon;
    SbBool clipInner;
    //@}

    /** @name Spinning data */
    //@{
    SbBool spinanimatingallowed;
    int spinsamplecounter;
    SbRotation spinincrement;
    SbRotation spinRotation;
    SbSphereSheetProjector * spinprojector;
    //@}

private:
    struct NavigationStyleP* pimpl;
    friend struct NavigationStyleP;
};

/** Sub-classes of this class appear in the preference dialog where users can
 * choose their favorite navigation style.
 * All other classes that inherit directly from NavigationStyle do not appear
 * in the above dialog.
 * This mechanism is useful to implement special navigation styles which are
 * only needed for certain purposes. Thus, it should not be possible to be
 * choosable by the user 
 * @author Werner Mayer
 */
class GuiExport UserNavigationStyle : public NavigationStyle {
    TYPESYSTEM_HEADER();

public:
    UserNavigationStyle(){}
    ~UserNavigationStyle(){}
    virtual const char* mouseButtons(ViewerMode) = 0;
};

class GuiExport InventorNavigationStyle : public UserNavigationStyle {
    typedef UserNavigationStyle inherited;

    TYPESYSTEM_HEADER();

public:
    InventorNavigationStyle();
    ~InventorNavigationStyle();
    const char* mouseButtons(ViewerMode);

protected:
    SbBool processSoEvent(const SoEvent * const ev);
};

class GuiExport CADNavigationStyle : public UserNavigationStyle {
    typedef UserNavigationStyle inherited;

    TYPESYSTEM_HEADER();

public:
    CADNavigationStyle();
    ~CADNavigationStyle();
    const char* mouseButtons(ViewerMode);

protected:
    SbBool processSoEvent(const SoEvent * const ev);

private:
    SbBool lockButton1;
};

class GuiExport BlenderNavigationStyle : public UserNavigationStyle {
    typedef UserNavigationStyle inherited;

    TYPESYSTEM_HEADER();

public:
    BlenderNavigationStyle();
    ~BlenderNavigationStyle();
    const char* mouseButtons(ViewerMode);

protected:
    SbBool processSoEvent(const SoEvent * const ev);

private:
    SbBool lockButton1;
};

class GuiExport TouchpadNavigationStyle : public UserNavigationStyle {
    typedef UserNavigationStyle inherited;

    TYPESYSTEM_HEADER();

public:
    TouchpadNavigationStyle();
    ~TouchpadNavigationStyle();
    const char* mouseButtons(ViewerMode);

protected:
    SbBool processSoEvent(const SoEvent * const ev);
};

} // namespace Gui

#endif // GUI_NAVIGATIONSTYLE_H 
