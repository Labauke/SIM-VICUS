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

#ifndef NM_AbstractControllerH
#define NM_AbstractControllerH

#include <cstddef>
#include <limits>

namespace NANDRAD_MODEL {

/*! Defines the interface for an abstract controller.
	This interface can be called from any model that provides abstract quantities.
	\code
		//derived class from AbstractController
		PController controller;
		// set parameters
		// ...
		// new calculation step:
		// compute error value as difference between target and current process value
		errorValue = setPointTemperatureDifference - temperatureDifference;
		// calculate new controller output
		controller.update(errorValue);
		// retrieve new control value
		double massFlux = contoller.m_controlValue
		// ...
	\endcode
*/
class AbstractController {
public:
	/*! D'tor, definition is in NM_Controller.cpp. */
	virtual ~AbstractController();

	/*! Calculates controller signal/control value. This function must be re-implemented in derived classes.
		Default implementation caches the errorValue. Call the base class implementation from derived classes.
		\param errorValue This is the input error value, i.e. the difference between
				desired setpoint and current process variable.
	*/
	virtual void update(double errorValue) { m_errorValue = errorValue; }

	/*! This function is called at the begin of an integrator step and signals the end time point
		of the current step.
		Use this function to compute the integration step size in case you want to integrate manually.
	*/
	virtual void setTime(double t) { (void)t; }

	/*! This function is called after each integration step (default implementation does nothing). */
	virtual void stepCompleted(double t) { (void)t; }

	/*! Computes and returns serialization size in bytes, by default returns  returns an invalid value (-1). */
	virtual std::size_t serializationSize() const { return 0; }

	/*! Stores model content at memory location pointed to by dataPtr.
	*/
	virtual void serialize(void* & dataPtr) const { (void)dataPtr; }

	/*! Restores model content from memory at location pointed to by dataPtr.
	*/
	virtual void deserialize(void* & dataPtr) { (void)dataPtr; }

	/*! sets error integral value to 0 (anti-windup), should be implemented for PI controllers */
	virtual void resetErrorIntegral() {}

	/*! Stores result of controller calculation, updated in each call to update().
		\note Value is initialized with dummy value to check for correct initialization during model init.
	*/
	double				m_controlValue = 888;
	/*! Cache of error value updated in last call to update(). */
	double				m_errorValue = 777;
	/*! Control value will be clipped below this value */
	double				m_controlValueMinimum = std::numeric_limits<double>::lowest();
	/*! Control value will be clipped above this value */
	double				m_controlValueMaximum = std::numeric_limits<double>::max();
};

} // namespace NANDRAD_MODEL

#endif // NM_AbstractControllerH
