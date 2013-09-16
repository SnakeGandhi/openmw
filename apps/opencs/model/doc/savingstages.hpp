#ifndef CSM_DOC_SAVINGSTAGES_H
#define CSM_DOC_SAVINGSTAGES_H

#include <components/esm/defs.hpp>

#include "stage.hpp"

#include "savingstate.hpp"

#include "../world/record.hpp"

namespace CSMDoc
{
    class Document;
    class SavingState;

    class OpenSaveStage : public Stage
    {
            Document& mDocument;
            SavingState& mState;

        public:

            OpenSaveStage (Document& document, SavingState& state);

            virtual int setup();
            ///< \return number of steps

            virtual void perform (int stage, std::vector<std::string>& messages);
            ///< Messages resulting from this stage will be appended to \a messages.
    };

    class WriteHeaderStage : public Stage
    {
            Document& mDocument;
            SavingState& mState;

        public:

            WriteHeaderStage (Document& document, SavingState& state);

            virtual int setup();
            ///< \return number of steps

            virtual void perform (int stage, std::vector<std::string>& messages);
            ///< Messages resulting from this stage will be appended to \a messages.
    };


    template<class CollectionT>
    class WriteCollectionStage : public Stage
    {
            const CollectionT& mCollection;
            SavingState& mState;
            ESM::RecNameInts mRecordType;

        public:

            WriteCollectionStage (const CollectionT& collection, SavingState& state,
                ESM::RecNameInts recordType);

            virtual int setup();
            ///< \return number of steps

            virtual void perform (int stage, std::vector<std::string>& messages);
            ///< Messages resulting from this stage will be appended to \a messages.
    };

    template<class CollectionT>
    WriteCollectionStage<CollectionT>::WriteCollectionStage (const CollectionT& collection,
        SavingState& state, ESM::RecNameInts recordType)
    : mCollection (collection), mState (state), mRecordType (recordType)
    {}

    template<class CollectionT>
    int WriteCollectionStage<CollectionT>::setup()
    {
        return mCollection.getSize();
    }

    template<class CollectionT>
    void WriteCollectionStage<CollectionT>::perform (int stage, std::vector<std::string>& messages)
    {
        CSMWorld::RecordBase::State state = mCollection.getRecord (stage).mState;

        if (state==CSMWorld::RecordBase::State_Modified ||
            state==CSMWorld::RecordBase::State_ModifiedOnly)
        {
            std::string type;
            for (int i=0; i<4; ++i)
                /// \todo make endianess agnostic (change ESMWriter interface?)
                type += reinterpret_cast<const char *> (&mRecordType)[i];

            mState.getWriter().startRecord (type);
            mCollection.getRecord (stage).mModified.save (mState.getWriter());
            mState.getWriter().endRecord (type);
        }
        else if (state==CSMWorld::RecordBase::State_Deleted)
        {
            /// \todo write record with delete flag
        }
    }


    class CloseSaveStage : public Stage
    {
            SavingState& mState;

        public:

            CloseSaveStage (SavingState& state);

            virtual int setup();
            ///< \return number of steps

            virtual void perform (int stage, std::vector<std::string>& messages);
            ///< Messages resulting from this stage will be appended to \a messages.
    };

    class FinalSavingStage : public Stage
    {
            Document& mDocument;
            SavingState& mState;

        public:

            FinalSavingStage (Document& document, SavingState& state);

            virtual int setup();
            ///< \return number of steps

            virtual void perform (int stage, std::vector<std::string>& messages);
            ///< Messages resulting from this stage will be appended to \a messages.
    };
}

#endif