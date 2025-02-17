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

#ifndef NM_DefaultStateDependencyH
#define NM_DefaultStateDependencyH

#include "NM_AbstractStateDependency.h"
//#include "NM_InputReferenceToVectorValuedTarget.h"

#include <IBK_Parameter.h>

namespace NANDRAD_MODEL {

/*!	This class declares provides a default implementation for a state
	dependend model.

	\todo Document/Refactor
*/
class DefaultStateDependency : public AbstractStateDependency {
public:

	/*! Model types for identifying calculation type.
	*/
	enum modelCalculationTypes {
		SteadyState	= 0x01,
		CyclicGroup = 0x02,
		ODE			= 0x04
	};

	/*! Standard constructor. */
	DefaultStateDependency(unsigned int modelTypeID);

	/*! Copies the m_inputReferences - vector into the external inputRefs object. */
	virtual void inputReferences(std::vector<InputReference>  & inputRefs) const override;

	/*! Returns vector with pointers to input values.
		Each element of the reference vector refers to an element in the m_inputReferences vector
		(at the same index position). Only use this function if the inputReferences are scalar valued.
		In this case a requested input quantity can be accessed using the enumeration value (quantity
		type).

		\code
		const double *quantityRef = inputValueRefs()[quantityType]
		\endcode

		In the case of multiple (vector valued) references better use the direct access via
		'inputValueRef(quantityType,id)' for retreiving a reference to a given
		quantity type and a source model id. Alternatively use 'inputValueRefs(quantityType)'
		that provides an iterator to the first value reference of a given quantity type.
	*/
	virtual void inputValueRefs(std::vector<const double *> & valueRefs) const { valueRefs = m_inputValueRefs; }

	/*! Function for access an input value reference to a scalar target via enumeration.*/
	const double *inputValueRef(int quantityType) const;

	/*! Function for access an input referenced to a vector valued target.
		It returns an iterator at the starting position for the requested quantity
		type inside the m_inputValueRefs vector.
	*/
	std::vector<const double *>::const_iterator inputValueRefs(int quantityType) const;

protected:

	/*! Resizes the m_inputReferences vector from the keyword list. This function is
		valid as long as only single reference values to one quantity are needed. In the
		case of multiple references overwrite this function!
	*/
	virtual void initInputReferences(const std::vector<AbstractModel*> & /*models*/) override { }

	/*! Returns a pattern of direkt computational dependencies between input values
		and result values of the current model (the first pair entry correspondss
		to the result value, the second one to the input value of a dependency).
	*/
	virtual void stateDependencies(std::vector< std::pair<const double *, const double *> > &resultInputValueReferences) const override;

	/*! Defines constraints for all result values sorted via value reference:
		< value reference , < minimum value (-inf if undefined), maximum value (inf if undefined)>
		Default implementation return an empty map
	*/
	virtual void constraints(std::map< const double *, std::pair<double, double> > &constraintsPerValueRef) const override;

#if 0
	/*! Function for access an input reference to a scalar target via enumeration and vector index.
		If the input reference is missing a new element is inserted into the m_inputRferences vector.*/
	InputReference &inputReference(int quantityType, int index = -1);
	/*! Function for constant access  an input reference to a scalar target via enumeration and vector index.
		If the input reference is missing an exception is thrown.*/
	const InputReference &inputReference(int quantityType, int index = -1) const;
#endif

	/*! Transforms a target name into a target identifier for the mapping of input references.
		Returns -1 for missing quantity.
	*/
	virtual int decodeInputReferenceTargeType(const std::string &targetName) const;

private:

	/*! Provides the object with references to requested input variables (persistent memory location). */
	virtual void setInputValueRefs(const std::vector<QuantityDescription> & resultDescriptions, const std::vector<const double *> & resultValueRefs) override;

//	/*! Model references defined by all models. */
//	std::vector<InputReferenceToVectorValuedTarget>	m_inputReferences;

	/*! Vector with pointer to input values.
		This vector is resized and filled by the framework.
		\sa inputValueRefs()
	*/
	std::vector<const double *>			m_inputValueRefs;
	/*! Offset of input values for each input reference target. */
	std::vector<unsigned int>			m_inputValueOffset;


	// grant access to private/protected members

	friend class NandradModelImpl;
	friend class StateModelGroup;
};


} // namespace NANDRAD_MODEL

#endif // NM_DefaultStateDependencyH
