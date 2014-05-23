/*
This file is part of the Fido Plugin for Retroshare.

Zero Reserve is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Zero Reserve is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with Zero Reserve. If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef FIDOPLUGIN_H
#define FIDOPLUGIN_H

#include <retroshare/rsplugin.h>
#include <retroshare-gui/mainpage.h>

class p3Fido;

extern "C" {
    void * RETROSHARE_PLUGIN_provide();
};

class FidoPlugin: public RsPlugin
{
    friend void * RETROSHARE_PLUGIN_provide();
public:
    FidoPlugin() ;
    virtual ~FidoPlugin() {}

    virtual void getPluginVersion(int& major,int& minor,int& svn_rev) const ;
    virtual void setPlugInHandler(RsPluginHandler *pgHandler);

    virtual std::string getShortPluginDescription() const ;
    virtual std::string getPluginName() const;
    virtual RsPQIService * rs_pqi_service() const;
    virtual void setInterfaces(RsPlugInInterfaces& interfaces);

    QDialog * qt_about_page() const;

private:
    mutable p3Fido * m_Fido;
    RsPluginHandler * m_PlugInHandler;

    static FidoPlugin * fidoPlugin;
};



#endif

