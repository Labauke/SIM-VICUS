/*	NANDRAD Solver Framework and Model Implementation.

	Copyright (c) 2012-today, Institut für Bauklimatik, TU Dresden, Germany

	Primary authors:
	  Andreas Nicolai  <andreas.nicolai -[at]- tu-dresden.de>
	  Anne Paepcke     <anne.paepcke -[at]- tu-dresden.de>

	This program is part of SIM-VICUS (https://github.com/ghorwin/SIM-VICUS)

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.
*/

#include "NM_Directories.h"

#include <functional>
#include <cstdlib>
#include <ctime>

#include <IBK_assert.h>
#include <IBK_FormatString.h>
#include <IBK_FileUtils.h>
#include <IBK_Exception.h>
#include <IBK_StringUtils.h>


namespace NANDRAD_MODEL {

void Directories::create(const IBK::Path & projectRootPath) {
	FUNCID(Directories::create);

	m_rootDir.clear();
	m_logDir.clear();
	m_varDir.clear();
	m_resultsDir.clear();
	m_slavesDir.clear();

	// create solver base directory
	if ( !projectRootPath.exists() ) {
		if ( !IBK::Path::makePath( projectRootPath) ) {
			throw IBK::Exception( IBK::FormatString("Cannot create directory '%1'. Perhaps missing priviliges?").arg(projectRootPath), FUNC_ID);
		}
	}

	IBK::Path logPath = projectRootPath / "log";
	if ( ! logPath.exists() ) {
		if ( !IBK::Path::makePath(logPath)) {
			throw IBK::Exception("Cannot create directory '" + logPath.str() + "'. Perhaps missing priviliges?", FUNC_ID);
		}
	}

	IBK::Path varPath = projectRootPath / "var";
	if ( !varPath.exists() ) {
		if ( !IBK::Path::makePath(varPath)) {
			throw IBK::Exception("Cannot create directory '" + varPath.str() + "'. Perhaps missing priviliges?", FUNC_ID);
		}
	}

	IBK::Path outPath = projectRootPath / "results";
	if ( !outPath.exists() ) {
		if ( !IBK::Path::makePath(outPath)) {
			throw IBK::Exception("Cannot create directory '" + outPath.str() + "'. Perhaps missing priviliges?", FUNC_ID);
		}
	}

	m_rootDir = projectRootPath;
	m_logDir = logPath;
	m_varDir = varPath;
	m_resultsDir = outPath;
}


void Directories::createFMUSlavesDirectory() {
	FUNCID(Directories::createFMUSlavesDirectory);

	m_slavesDir.clear();

	IBK_ASSERT(m_rootDir.exists());

	IBK::Path slavePath = m_rootDir / "slaves";
	if (!slavePath.exists()) {
		if (!IBK::Path::makePath(slavePath)) {
			throw IBK::Exception("Cannot create directory '" + slavePath.str() + "'. Perhaps missing priviliges?", FUNC_ID);
		}
	}

	m_slavesDir = slavePath;
}


} // namespace NANDRAD_MODEL

