#ifndef SVUndoModifyRoomSoundProtectionTemplateAssociationH
#define SVUndoModifyRoomSoundProtectionTemplateAssociationH


#include <vector>

#include "SVUndoCommandBase.h"

#include <VICUS_Room.h>


class SVUndoModifyRoomAcousticTemplateAssociation : public SVUndoCommandBase {
	Q_DECLARE_TR_FUNCTIONS(SVUndoModifyRoomSoundProtectionTemplateAssociation)
public:
	/*! Replaces zone template IDs in all rooms identified by their unique IDs. */
	SVUndoModifyRoomAcousticTemplateAssociation(const QString & label,
												const std::vector<unsigned int> & roomIDs,
												unsigned int acousticTemplateID);

	/*! The modification data object passed along with the undo action. */
	class Data : public ModificationInfo {
	public:
		std::vector<const VICUS::Object*> m_objects;
	};

	virtual void undo();
	virtual void redo();

private:
	/*! Data member to hold modified room IDs vector, these are the uniqueIDs of the rooms!. */
	std::vector<unsigned int> m_roomIDs;
	std::vector<unsigned int> m_acousticTemplateIDs;
};

#endif // SVUndoModifyRoomAcousticTemplateAssociationH
