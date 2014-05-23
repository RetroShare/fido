/*
    This file is part of the Fido Plugin for Retroshare.

    Fido is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Fido is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with Fido.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "p3Fido.h"

#include "pqi/p3linkmgr.h"
#include "retroshare/rsturtle.h"

#include <QDir>
#include <QStringList>

#include <fstream>
#include <iostream>


static const uint16_t RS_SERVICE_TYPE_FIDO_PLUGIN = 0xBEEE;
static const uint32_t CONFIG_TYPE_FIDO_PLUGIN     = 0xDEADBEEE;

p3Fido::p3Fido( RsPluginHandler *pgHandler ) :
    RsPQIService( RS_SERVICE_TYPE_FIDO_PLUGIN, CONFIG_TYPE_FIDO_PLUGIN, 0, pgHandler )
{
}

int p3Fido::tick()
{
    pollMaildir();
    return 0;
}


void p3Fido::pollMaildir()
{
    QString home = getenv( "HOME" );
    QString maildirPath = home + "/Maildir/new";
    QDir maildir( maildirPath );
    if( !maildir.exists() ){
        std::cerr << "Fido: Maildir " << maildirPath.toStdString() << " does not exist!" << std::endl;
        return;
    }
    QStringList files = maildir.entryList();

    for( QStringList::ConstIterator it = files.begin(); it != files.end(); it++ ){
        QString filename = *it;
        if( filename[0] == '.' ) continue;
        std::cerr << "Fido: Maildir entry: " << filename.toStdString() << std::endl;
    }
}
