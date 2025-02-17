/*	QtExt - Qt-based utility classes and functions (extends Qt library)

	Copyright (c) 2014-today, Institut für Bauklimatik, TU Dresden, Germany

	Primary authors:
	  Heiko Fechner    <heiko.fechner -[at]- tu-dresden.de>
	  Andreas Nicolai

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.

	Dieses Programm ist Freie Software: Sie können es unter den Bedingungen
	der GNU General Public License, wie von der Free Software Foundation,
	Version 3 der Lizenz oder (nach Ihrer Wahl) jeder neueren
	veröffentlichten Version, weiter verteilen und/oder modifizieren.

	Dieses Programm wird in der Hoffnung bereitgestellt, dass es nützlich sein wird, jedoch
	OHNE JEDE GEWÄHR,; sogar ohne die implizite
	Gewähr der MARKTFÄHIGKEIT oder EIGNUNG FÜR EINEN BESTIMMTEN ZWECK.
	Siehe die GNU General Public License für weitere Einzelheiten.

	Sie sollten eine Kopie der GNU General Public License zusammen mit diesem
	Programm erhalten haben. Wenn nicht, siehe <https://www.gnu.org/licenses/>.
*/

#include "QtExt_Constants.h"

namespace QtExt{

unsigned int PANEL_BUTTON_SIZE = 14;
unsigned int SOUTH_PANEL_HEIGHT = 50;
unsigned int EAST_PANEL_WIDTH = 350;
unsigned int WEST_PANEL_WIDTH = 400;

/*! The font size for the tables. */
unsigned int TABLE_VIEW_FONT_SIZE = 8;

QColor WINDOW_COLOR_DARKGRAY = QColor(73,73,73,255);
QColor WINDOW_COLOR_GRAY = QColor(57,57,57,255);

QColor ColorHeat						= QColor("#e00000");
QColor ColorVapor						= QColor("#5e8deb");
QColor ColorWater						= QColor("#0000d5");
QColor ColorAir							= QColor("#aecccc");
QColor ColorVOC							= QColor("#442222");
QColor ColorSalt						= QColor("#aaaaaa");

}
