/********************************************************************
filename:   WXLogicModel.h

purpose:    LogicModel���������滻ԭ��DObject�ģ�����Ҫ��������Ⱦ����
			�ͻ��˵�һ���Ӻϲ㣬������Ⱦ�㣬ʵ�ֿͻ��˶Գ�����
			��̬�����һЩ�����绻װ������Ч�ȣ��������ǿͻ��˿��Ժ�
			�����ʵ�ָ���Ч���������������Ⱦ��Ĵ��롣
*********************************************************************/

#ifndef _FAIRYLOGICMODEL_H_
#define _FAIRYLOGICMODEL_H_

#include "Core/WXPrerequisites.h"
#include "Core/WXVariant.h"
#include "Core/TerrainData.h"

#include "WXLogicModelManager.h"

#include "EffectSystem/WXEffect.h"

#include <OgreVector3.h>
#include <OgreQuaternion.h>
#include <OgreAny.h>
#include <OgreStringVector.h>
#include <OgreMaterial.h>
#include <OgreRay.h>
#include <OgreIteratorWrappers.h>

// �������й����еĻص�����
typedef bool          (*OnAnimationFinish)(const char* pAnimationName, unsigned long info);

typedef bool          (*OnSkillHitTime)(const char* pAnimationName, unsigned long info,
                                        unsigned short index, unsigned short totalCount);

typedef bool          (*OnSkillBreakTime)(const char* pAnimationName, unsigned long info,
                                          unsigned short index, unsigned short totalCount);

typedef bool          (*OnSkillShakeTime)(const char* pAnimationName, unsigned long info);
typedef bool          (*OnGetEffectName)(const char* pAnimationName, Ogre::String& effectName);


// ��ȡ��Ϸ������ĳһx��z����ĸ߶�ֵ
typedef bool         (__stdcall*GetHeightInWorld)(float x, float z, float& y);

// ��������ָ��
typedef int (__stdcall* OnPlaySound)(const char* szSoundFile, float* fvPos, bool isLoop);
typedef void (__stdcall* OnStopSound)(int handle);

namespace Ogre	{

	class Entity;
	class Any;
	class UserDefinedObject;
	class SkeletonInstance;
	class Node;
	class AnimationState;
	class Animation;
    class AxisAlignedBox;

    /// ���������������
    class Fabric;
    class StdCollider;
    class ColliderSet;
}

namespace WX	{

    const Ogre::ColourValue& _getColourFromMaterial(const Ogre::MaterialPtr& mat);

    enum ModelProjectorType
    {
        MPT_SELECTED,   // ��ѡ��ʱ��ʾ�ĵ�projector
        MPT_SHADOW,     // ���µļ���Ӱ
    };

	class System;
	class Effect;
	class Skill;
   // class LogicModelManager;

	class LogicModel
	{
        // ����LogicModelManagerΪ��Ԫ�ֱ࣬�Ӵ洢˽�г�Ա
        friend class LogicModelManager;

	public:

		/// ��������ʱ����������
		static String MESH_HINT;
		static String MATERIAL_HINT;
		static String SLMT_CREATE_HINT;

		/// ������������
		static String MOVEMENT_TYPE_ATTRIBUTE;
        static String DEFAULT_ANIM_NAME;
        static String BOUNDING_BOX;
        static String SCALE_FACTOR;

        static String HAIR_MESH_NAME;

		/// addEffectʱ���ص���Ч���
		typedef uint EffectHandle;

		/// ��ȡlocator��Ϣʱ�ı�־
		enum GetTransformInfoType
		{
			GTIT_POSITION,	// ֻ��ȡλ����Ϣ
			GTIT_ORIENTATION,	// ֻ��ȡ��ת��Ϣ
			GTIT_ALL,	// ��ȡλ����Ϣ����ת��Ϣ
		};

		// object���ͣ���Ҫ������object�����߷�ʽ
		enum MovementType
		{
			MT_NORMAL, // ��ͨ���ͣ��������������object�ķ�λ
			MT_ROTATE_X_AXIS,	// ֻ��ǰ��ҡ��
			MT_ROTATE_X_Z_AXIS,	// ����ǰ������ҡ��
		};

		/// �Ѵ�����effect����Ϣ������updateAllEffects
		struct CreatedEffectInfo
		{
			CreatedEffectInfo() :
			mEffect(NULL),
			mLocator(""),
            mTransformType(GTIT_POSITION),
            mEffectHandle(0)
			{
			}

			CreatedEffectInfo(Effect* effect, const String& locatorName, 
                GetTransformInfoType transformType, EffectHandle handle) :
			mEffect(effect),
			mLocator(locatorName),
            mTransformType(transformType),
            mEffectHandle(handle)
			{
			}

			Effect* mEffect;
			String mLocator;
            GetTransformInfoType mTransformType;
            EffectHandle mEffectHandle;
		};

        typedef std::list<CreatedEffectInfo> CreatedEffectList;

        /// slot����
        struct SlotValue
        {
            SlotValue() : mModelName(""), mModel(NULL)
            {
            }

            SlotValue( const String& modelName ) :
            mModelName(modelName),
                mModel(NULL)
            {
            }

            ~SlotValue(void)
            {
                if (mModel)
                    LogicModelManager::getSingleton().destroyLogicModel(mModel);
            }

            String mModelName;
            LogicModel* mModel;
        };

        typedef std::map<String, SlotValue> SlotMap;

        /// locator��Ϣ
        struct	LocatorValue
        {
            LocatorValue() : 
            mBoneName(""),
                mOffsetPos(Ogre::Vector3::ZERO),
                mOffsetOrientation(Ogre::Quaternion::IDENTITY),
                mLocatorNode(NULL),
                mTransferable(false),
                mTranslateFirst(false)
            {
                mSlotMap.clear();
            }

            LocatorValue(const String& boneName, const Ogre::Vector3& pos, 
                const Ogre::Quaternion& orientation, bool transferable = false,
                bool translateFirst = false) :
            mBoneName(boneName),
                mOffsetPos(pos),
                mOffsetOrientation(orientation),
                mLocatorNode(NULL),
                mTransferable(transferable),
                mTranslateFirst(translateFirst)
            {
                mSlotMap.clear();
            }

            LocatorValue& operator = (const LocatorValue& rhs)
            {
                this->mBoneName = rhs.mBoneName;
                this->mOffsetPos = rhs.mOffsetPos;
                this->mOffsetOrientation= rhs.mOffsetOrientation;
                this->mLocatorNode = rhs.mLocatorNode;
                this->mTransferable = rhs.mTransferable;
                this->mTranslateFirst = rhs.mTranslateFirst;

                return *this;
            }

            String	mBoneName;
            Ogre::Vector3		mOffsetPos;
            Ogre::Quaternion	mOffsetOrientation;
            Ogre::SceneNode*	mLocatorNode;
            SlotMap			mSlotMap;
            bool mTransferable;
            bool mTranslateFirst;   // ��update locator posʱ�Ƿ��Ƚ���translate
        };

        typedef std::map<String, LocatorValue> LocatorMap;

        /// ������ײ������
        enum FabricColliderType
        {
            FCT_BOX,
            FCT_SPHERE,
            FCT_PLANE,
        };

        /// ������ײ���������Ϣ
        struct FabricCollider 
        {
            FabricCollider(const String& locator, FabricColliderType type, const String& colliderInfo) :
            mLocatorName(locator), mColliderType(type), mColliderInfo(colliderInfo),
            mLocatorNode(NULL), mCollider(NULL)
            {
            }

            String mLocatorName;    // ��ײ������Ĺҽӵ������
            FabricColliderType mColliderType;   // ��ײ������
            String mColliderInfo;   // ��ײ��Ķ�����Ϣ�����ݲ�ͬ��ײ���������box���͵���Ҫ���������������峤���ߡ�
            Ogre::SceneNode* mLocatorNode;  // ����locator��ָ��
            Ogre::StdCollider* mCollider;   // ������������ײ���ָ��
        };

        /// ���沼����ײ����Ϣ��map
        typedef std::map<String, FabricCollider> FabricColliderMap;

        /// ��ɫ������ֵ
        struct AttribValue
        {
            AttribValue() : mAttribValue(""), mHint(0)
            {
            }

            AttribValue(const String& value, const String& hint) :
            mAttribValue(value),
                mHint(hint)
            {
            }

            String mAttribValue;
            String mHint;
        };
        /// ����map��key����������value������ֵ��Ϣ
        typedef std::map<String, AttribValue> AttribMap;

	protected:

        /// ͸�������õĲ���ʵ��
        struct ModelMaterialInstance
        {
            ModelMaterialInstance(const Ogre::MaterialPtr& origin);
            ~ModelMaterialInstance();

            Ogre::MaterialPtr mOrigin;
            Ogre::MaterialPtr mDerived;
        };

        /// ͸����
        class EntityTransparentor
        {
        protected:
            Ogre::Entity* mEntity;

            typedef std::vector<ModelMaterialInstance*> MaterialInstanceList;
            typedef std::vector<size_t> IndexList;
            size_t addMaterial(const Ogre::MaterialPtr& material);

            bool applyTransparency(const Ogre::MaterialPtr& origin, const Ogre::MaterialPtr& derived, Real transparency);

        public:
            EntityTransparentor(void);

            EntityTransparentor(Ogre::Entity* entity);

            ~EntityTransparentor();

            void clear(void);

            void init(Ogre::Entity* entity);

            void apply(Real transparency);

        public:
            MaterialInstanceList mMaterialInstances;
            IndexList mSubEntityMaterialIndices;
        };

		/** typedef	*/
		
		/// ��ͨ��ɫ������entity���б�
		struct EntityValue
		{
			EntityValue() : mMeshName(""), mMaterialName(""), mEntity(NULL), mTransparentor(NULL),
                mEntityColour(Ogre::ColourValue::Black), mEntityOriginColour(Ogre::ColourValue::Black),
                mEntityColourChanged(false)
			{
			}

			EntityValue(const String& meshName, const String& matName) :
			mMeshName(meshName),
				mMaterialName(matName),
				mEntity(NULL),
                mTransparentor(NULL),
                mEntityColour(Ogre::ColourValue::Black),
                mEntityOriginColour(Ogre::ColourValue::Black),
                mEntityColourChanged(false)
			{
			}

			String mMeshName;
			String mMaterialName;
			Ogre::Entity* mEntity;
            EntityTransparentor* mTransparentor;
            /// ����clone���Ĳ���
            std::vector< Ogre::MaterialPtr > mClonedMaterials;
            /// ����clone֮ǰ�õĲ���
            std::vector< String > mOriginMaterialNames;

            /// ���entity�����õ���ɫֵ
            Ogre::ColourValue mEntityColour;
            /// entity������ɫ֮ǰ����ɫֵ
            Ogre::ColourValue mEntityOriginColour;
            bool mEntityColourChanged;
		};

		typedef std::map<String, EntityValue> EntityMap;

		/// material����
		struct MaterialValue
		{
			MaterialValue() : mEntityEntry(""), mMaterialName("")
			{
			}

			MaterialValue(const String& entityEntry, const String& matName) :
			mEntityEntry(entityEntry),
			mMaterialName(matName)
			{
			}

			String mEntityEntry;
			String mMaterialName;
		};

		typedef std::map<String, MaterialValue> MaterialMap;

		/// ��¼obj�ļ���Effects�����Ϣ
		struct EffectValue
		{
            EffectValue() :	mEffectName(""), mLocator(""), mTranslateAll(false), mEffectColour(Ogre::ColourValue::White)
			{
			}

            EffectValue(const String& effectName, const String& locatorName, bool translateAll, const Ogre::ColourValue& colour) :
			mEffectName(effectName),
				mLocator(locatorName),
                mTranslateAll(translateAll),
                mEffectColour(colour)
			{
			}

			String mEffectName;
			String mLocator;
            bool mTranslateAll;
            Ogre::ColourValue mEffectColour;
		};
		typedef std::map<String, EffectValue> EffectMap;			

		/// ���ҽ��ϵ�����ԭ����node
		typedef std::map<LogicModel*, Ogre::Node*> AttachedModelOriginSceneNodes;

        /// ����model�ϵ�projector
        struct ModelProjector
        {
            ModelProjector() : mProjectorName(""), mType(MPT_SELECTED), mProjectorEffect(NULL), mProjectorNode(NULL)
            {
            }

            ModelProjector(const String& projectorName, ModelProjectorType type) :
            mProjectorName(projectorName),
            mType(type),
            mProjectorEffect(NULL),
            mProjectorNode(NULL)
            {
            }

            String mProjectorName;  // ������Ч����
            ModelProjectorType mType;   // ͶӰ����
            Effect* mProjectorEffect;   // ��Чָ��
            Ogre::SceneNode* mProjectorNode;
        };
        typedef std::list<ModelProjector*> ModelProjectors;        

        /// ��ҪͶ��Ӱ��mesh����
        typedef std::list<String> ShadowUncastableMeshs;
        
		/// ������붯�����б�
        typedef std::list<Ogre::AnimationState*> PlayingAnimationList;
        PlayingAnimationList mPlayingAnimationList;

        /// ����ר��͸����
        class FabricTransparentor
        {
        protected:
            Ogre::Fabric* mFabric;
            ModelMaterialInstance* mMaterialInstance;

            bool applyTransparency(const Ogre::MaterialPtr& origin, const Ogre::MaterialPtr& derived, Real transparency);

        public:
            FabricTransparentor(Ogre::Fabric* fabric);
            ~FabricTransparentor();

            void clear(void);
            void init(void);
            void apply(Real transparency);
        };

        struct FabricInfo
        {
            FabricInfo(Ogre::Fabric* fabric)
                : mFabric(fabric)
                , mTransparentor(NULL)
            {
            }

            Ogre::Fabric* mFabric;
            FabricTransparentor* mTransparentor;
        };

        typedef std::map<String, FabricInfo> FabricInfoMap;

	public:
		LogicModel( const String& name, System *system );
		~LogicModel();

		/// Model����л��գ��ڻ���ǰҪ��reset
		void reset(void);

		/// obj�ļ������Ե�����
		bool addAttribute( const String &name, const String &value, const String &hint );
		bool changeAttribute( const String &name, const String &value );

        Variant getAttribute( const String& attributeName );

        const AttribMap& getAttribMap(void)
        {
            return mAttribMap;
        }
        /** ��ȡ��ǰ����ֵ
        @param value ��ǰ����ֵ
        @return 
            ���û��Ҫ�ҵ����ԣ��ͷ���false
        */
        bool getAttribute( const String& name, String& value);

        /** ����Ƿ���ָ��������
        */
        bool hasAttribute( const String& name );

		bool addEntity( const String &name, const String &meshName, const String &matName );

        /** �ı�entity
        @param name ��������
        @param value mesh���ƣ����Ϊ""����ʾɾ��ԭ����entity���������µ�entity
        */
		bool changeEntity( const String &name, const String &value );

		bool addMaterial( const String &name, const String &entityEntry, const String &matName );
		bool changeMaterial( const String &name, const String &value );

		bool addLocator( const String &name, const String &boneName, 
			const Ogre::Vector3 &pos, const Ogre::Quaternion &orientation, bool transferable = false, bool translateFirst = false);

        /// �ж��������locator�Ƿ�Ϊtransferable
        bool isLocatorTransferable(const String& name);

        bool isLocatorExist(const String& locator);

        /// ��ȡlocator��Ϣ
        bool getLocatorInfo(const String& locator, LocatorValue& locatorInfo);
        bool setLocatorInfo(const String& locator, const LocatorValue& info);

		bool addSlot( const String &locatorName, const String &slotName, const String &modelName, const String &attrib );
		bool changeSlotAttrib( const String &name, const String &value );
		//LogicModel& operator = (const LogicModel& rhs);

        /** ������Ч��Ϣ����obj�ļ��л�ȡ����Ϣ��
        */
		bool addEffectInfo( const String& name, const String& effectName,
            const String& locatorName, bool translateAll = false, const Ogre::ColourValue& colour = Ogre::ColourValue::White );

        /** ���Ӳ�����͸����Ӱ��mesh����
        */
        bool addShadowUncastable(const String& meshName);

        /// �жϸ�mesh�Ƿ�涨Ϊ����ͶӰ
        bool isShadowUncastable(const String& meshName);

		void setUserAny(const Ogre::Any& anything);
		const Ogre::Any& getUserAny(void) const;

		void setUserObject(Ogre::UserDefinedObject* obj);
		Ogre::UserDefinedObject* getUserObject(void) const;

		void setParentSceneNode( Ogre::Node* parentNode );		
		Ogre::SceneNode* getSceneNode(void)
		{
			return mModelMainNode;
		}

		void destroySceneNode(void);

		void destroyEntities(void);

		const Ogre::AxisAlignedBox&     getBoundingBox(void);

		void initModel(void);

		/// ÿ֡�н��б�Ҫ�ĸ��£��綯������Ч��skill�ĸ���
		void execute(Real delta);

		/// ����ȫ�ֵĶ������ʣ�1.0��ʾ1���٣�2.0��ʾ2����
		void setGlobalAnimationRate(Real rate)
		{
			mGlobalAnimationRate = rate;
		}
		Real getGlobalAnimationRate(void)
		{
			return mGlobalAnimationRate;
		}

		/// �������model������entity��visible flag
		void setVisibleFlag(uint32 flags);
        uint32 getVisibleFlag(void)
        {
            return mVisibleFlag;
        }

		void setAnimationLoop(bool loop);
		bool getAnimationLoop(void)
		{
			return mAnimationLoop;
		}

        /** ��LogicModel�Ϲ�һ��effect
        @param effectName ��Ч����
        @param locatorName �ҽӵ�����
        @param transformType ��Ч���˶����ͣ�Ĭ��ΪGTIT_POSITION����ʾ
               ֻ����model��λ����Ϣ
        @param colour ������Чʱ��ָ������Ч��ɫֵ
        */
		EffectHandle addEffect( const String& effectName, const String& locatorName, 
            GetTransformInfoType transformType = GTIT_POSITION, const Ogre::ColourValue& colour = Ogre::ColourValue::White );

        /** ɾ��effect
        @param handle ��Чhandle
        @remarks �����������û�ҽ���ָ����effect�����쳣
        */
        CreatedEffectList::iterator delEffect( EffectHandle handle );

        /** ɾ��effect
        @param effectIterator ָ��Ҫɾ������Ч��iterator
        */
        CreatedEffectList::iterator delEffect( CreatedEffectList::iterator effectIterator );

        /// ɾ��������Ч
		void delAllEffect(void);

        /** ��ȡָ����Ч
        @param handle ������Ч��handle
        @remarks �����ǰ��������û�����effect���ͷ���NULL
        */
        Effect* getEffect(EffectHandle handle);

		bool getLocatorWorldTransform(const String& name, WX::TransformInfo& info, const Ogre::Vector3& offsetPos = Ogre::Vector3::ZERO,
			const Ogre::Quaternion& offsetRotation = Ogre::Quaternion::IDENTITY, GetTransformInfoType type = GTIT_ALL );
		bool getBoneWorldTransform(const String& name, WX::TransformInfo& info, const Ogre::Vector3& offsetPos = Ogre::Vector3::ZERO,
			const Ogre::Quaternion& offsetRotation = Ogre::Quaternion::IDENTITY, GetTransformInfoType type = GTIT_ALL );

        bool getLocatorWorldTransform(const String& name, Ogre::Matrix4& mtx);

		void createSkill(const String& skillName, bool loop = true, bool anim = false, Ogre::Real delayTime = 0.0f);
		void delCurrentSkill(void);

		Skill* getCurrentSkill(void)
		{	
			return mCurrentSkill;
		}

		void setName(const String& name)
		{
			mName = name;
		}
		const String& getName(void)
		{
			return mName;
		}		

		bool attachModel(const String& locatorName, LogicModel* model);
		bool detachModel( LogicModel* model );
        /// ����attach�Լ���LogicModel
        void notifyAttach(LogicModel* parentModel)
        {
            mAttachParent = parentModel;
        }

        /// �жϵ�ǰ�Ƿ�ҽ��ڱ��model��
        bool isAttached(void)
        {
            return (mAttachParent != NULL);
        }

		/// ��ȡ��ǰ�ҽ�model�ĸ�model
        LogicModel* getAttachModel(void)
        {
            return mAttachParent;
        }

		bool isChildModel(void)
		{
			return mChildModel;
		}

		void setChildModel(bool child)
		{
			mChildModel = child;
		}

		void setPosition(Ogre::Vector3& pos);

		Ogre::Vector3& getPosition(void)
		{
			return mModelPosition;
		}

		void setOrientation(Ogre::Quaternion& orientation);

		Ogre::Quaternion& getOrientation(void)
		{
			return mModelOrientation;
		}

        bool isMainNodeNeedUpdate(void) const
        {
            return mMainNodeNeedUpdate;
        }

		// ������ݵ�������ת�ĽǶ�
		void calZAxisRotation(float deltaX, float deltaZ, float deltaHeight);
		void calXAxisRotation(float deltaX, float deltaZ, float deltaHeight);

        const String& getCurrentAnimName(void) const
        {
            return mCurrentAnimationName;
        }

        /// ����λ��
        void resetPosition(void);

        /// ����model�Ƿ���ʾ�������ҽ���locator�ϵ������Լ�attachModel����ȥ������
        void setVisible(bool visible);
        
        /** ��model������projector
        @remarks �ͻ��˵��ã����ڹҽ�projector���������µ���Ӱ��ѡ�л���
        @param type projector����
        @param projectorEffectName ��Ч����
        @param show �Ƿ��ڹ���ʱ����ʾ
        @param projectorSize ��ʼ��͸�䷶Χ
        */
        void addProjector(ModelProjectorType type, const String& projectorEffectName,
            bool show = true, float projectorSize = 100.0f);

        /** ɾ������model�ϵ�projector
        @param type projector����
        @param projectorEffectName ��Ч����
        @return ���ɾ���ɹ�,��return true,��Ȼ,��return false
        */
        bool removeProjector(ModelProjectorType type, const String& projectorEffectName);

        /** ��������model��͸����
        @remarks ����������Ըı�model��͸���ȣ����������Ϲҵ����壩
        @param time �ӵ�ǰ͸���ȹ��ɵ�transparent����������͸���ȵĹ���ʱ��
        @param transparency ��Ҫ��͸���ȣ�0��ʾ��͸����1��ʾȫ͸����Ҳ���ǲ��ɼ���
        */
        void setTransparent(Real time, Real transparency);

        /// ����model������entity��͸����
        void updateTransparency(Real deltaTime);

        /** ����͸������Ϣ
        @remarks ���ø�model��͸������Ϣ�����transparentTimeΪ0�����������õ�entity�У����򣬾���Ҫ��execute�н��й���
        @param currTranparency ��ǰʱ�̵�͸����
        @param destTranparency Ҫ�ﵽ��͸����
        @param transparentTime ��ǰ��ʣ�µ�͸������ʱ��
        */
        void setTransparencyInfo(Real currTransparency, Real destTransparency, Real transparentTime);

        void setHairColour(const Ogre::ColourValue& colour);

        Ogre::ColourValue& getHairColour(void)
        {
            return mHairColour;
        }

        /// �����Ƿ����ͶӰ
        void setShadowCastable(bool castable);

        /** ���������Ƿ�ѡ��
        @param selected ���ѡ�У�Ĭ�ϵ�����Ǹ�������Է������0.5
        */
        void setSelected(bool selected);

        /// ��ǰ�����Ƿ���ѡ��״̬
        bool isSelected(void)
        {
            return mSelected;
        }

        /** ����������ɫ��ͷ�����⣩
        @param colour Ҫ���õ���ɫֵ
        @param method ��ɫ���÷�ʽ
        */
        void setBodyColour( const Ogre::ColourValue& colour );

        /// ����������ɫ��ԭʼ����
        void resetBodyColour(void);

        /** �ѵ�ǰ��bounding box��ray�����ཻ�Լ��
        @param ray ���м�������
        @return �������ray�͵�ǰ��bounding box�ཻ���ͷ���true�����򣬷���false
        */
        bool rayIntersect(const Ogre::Ray& ray);

        /// ��ǰ��bounding box�Ƿ����ⲿ�����
        bool isUseExternalBoundingBox(void)
        {
            return NULL != mExternalBoundingBox;
        }

        /** ����һ������
        @param fabricName ���ϵ����ƣ������ڸ�model����Ψһ�ģ����֮ǰ�Ѿ�������
                          ͬ���Ĳ��ϣ����֮ǰ�Ĳ���ɾ��
        @param headLocatorName ��һ�����ҽӵ������
        @param tailLocatorName ���һ���ҽӵ������
        @remarks
        */
        void addFabric(
            const String& fabricName,
            int width, int height,
            Real mass, Real naturalLength,
            Real structuralStiffness,
            Real shearStiffness,
            Real flexionStiffness,
            Real deformationRate,
            const Ogre::Vector3& windVelocity,
            const String& materialName,
            const String& headLocatorName,
            const String& tailLocatorName);

        /// ɾ��ָ�����ƵĲ���
        void removeFabric(const String& fabricName);

        /// ɾ����ǰ���ҽӵ����в���
        void removeAllFabrics(void);

        /** ����һ��������ײ��
        @param colliderName ��ײ�������ƣ�model��Ψһ����obj�ļ��ж���
        @param locatorName ��ײ�����ҽӵ�locator������
        @param colliderType ��ײ��������
        @param colliderInfo ������ײ������Ķ�����Ϣ����box�ĳ�����
        */
        bool addFabricCollider( const String& colliderName, const String& locatorName,
                                FabricColliderType colliderType, const String& colliderInfo = "" ); 

        /** �Ѳ�����ײ����������collider set��remove��
        @param colliderName ��ײ�������ƣ����Ϊ��TerrainCollider������remove������collider��
        ����ָ��������obj�ļ���FabricCollider��������� 
        */
        void removeFabricCollider( const String& colliderName );

        void enableAnimationState(bool enable = true);

        void setEnableWalkingEffect(bool enable)
        {
            mEnableWalkingEffect = enable;
        }
        bool getEnableWalkingEffect(void)
        {
            return mEnableWalkingEffect;
        }

        const Ogre::AxisAlignedBox& _getFullBoundingBox(void)
        {
            if (!mFullBoundingBox)
                mFullBoundingBox = new Ogre::AxisAlignedBox;

            return *mFullBoundingBox;
        }

        /// ��ǰ������Ƿ��ܿ�����model
        bool isCameraVisible(void);
        
        /// ��ȡentitymap�е�mesh����
        Ogre::StringVector getOriginMeshNames(void);

        /** ���ϲ���һ������
        @param animName ��������
        @param startTime ������ʼ���ŵ�ʱ��
        @param weight �ö�����Ȩ��
        @remarks ����ö��������ڣ�����log��д�������Ϣ
        */
        void playAnimation(const String& animName, Real startTime = 0.0f, Real weight = 1.0f);

        /// ��ȡָ��������length
        float getAnimationLength(const String& animName) const;

        /// ���ø�model��active״̬,������Ǵ���active״̬,�Ͳ�ˢ��
        void setActive(bool active = true)
        {
            mIsActive = active;
        }
        bool isActive(void)
        {
            return mIsActive;
        }

        /** ���õ�ǰ�����ŵĶ�����ָ����ʱ�俪ʼ����
        @param range ������Χ��0��1��0��ʾ���Ŷ����ĵ�һ֡��1��ʾ���������һ֡
        */
        void setCurrentAnimTime(Real range);

        /// ���õ�ǰģ�͵�����ϵ��
        void setScaleFactor(const Ogre::Vector3& scaleFactor);

        /// ���µ�ǰ���е�entity��normalize normal״̬
        void setEntityNormalizeNormal(bool normal);

		//////////////////////////////////////////////////////////////////////////
		void SetAnimationFinishListener(OnAnimationFinish func, ulong info)
		{
			mOnAnimationFinish = func;
            mCallbackFuncInfo = info;
		}
		OnAnimationFinish GetAnimationFinishListener(void)
		{
			return mOnAnimationFinish;
		}

		void SetSkillHitTimeListener(OnSkillHitTime func, ulong info)
		{
			mOnSkillHitTime = func;
            mCallbackFuncInfo = info;
		}
		OnSkillHitTime GetSkillHitTimeListener(void)
		{
			return mOnSkillHitTime;
		}

		void SetSkillBreakTimeListener(OnSkillBreakTime func, ulong info)
		{
			mOnSkillBreakTime = func;
            mCallbackFuncInfo = info;
		}
		OnSkillBreakTime GetSkillBreakTimeListener(void)
		{
			return mOnSkillBreakTime;
		}

        void SetSkillShakeTimeListener(OnSkillShakeTime func, ulong info)
        {
            mOnSkillShakeTime = func;
            mCallbackFuncInfo = info;
        }
        OnSkillShakeTime GetSkillShakeTimeListener(void)
        {
            return mOnSkillShakeTime;
        }

        void SetGetEffectNameListener(OnGetEffectName func, ulong info)
        {
            mOnGetEffectName = func;
            mCallbackFuncInfo = info;
        }
        OnGetEffectName GetGetEffectNameListener(void)
        {
            return mOnGetEffectName;
        }

        //////////////////////////////////////////////////////////////////////////
		static void SetGetHeightInWorldListener(GetHeightInWorld func)
		{
			mGetHeightInWorld = func;
		}
		static GetHeightInWorld GetGetHeightInWorldListener(void)
		{
			return mGetHeightInWorld;
		}

		static void SetPlaySoundFuncton(OnPlaySound func)
		{
			mOnPlaySound = func;
		}
		static OnPlaySound GetPlaySoundFuncton(void)
		{
			return mOnPlaySound;
		}

		static void SetStopSoundFunction(OnStopSound func)
		{
			mOnStopSound = func;
		}
		static OnStopSound GetStopSoundFunction(void)
		{
			return mOnStopSound;
		}

		/////////////////// �༭���� ////////////////////////
		ushort getNumBones(void);
		const String& getBoneName( ushort index );

		Real getCurrentAnimationLength(void);

		ushort getSkeletonAnimationCount(void);

		Ogre::Animation* getSkeletonAnimation( ushort index );

		void getLocatorNames( Ogre::StringVector& names );

		bool getAnimEditing(void)
		{
			return mAnimEditing;
		}
		void setAnimEditing(bool edit = true)
		{	
			mAnimEditing = edit;
		}

		void setAnimTimePos(Real sliderValue)
		{
			mAnimTimePos = sliderValue;
		}
		Real getAnimTimePos(void)	
		{
			return mAnimTimePos;
		}

		Real getCurrentTimePos(void);	

        typedef Ogre::MapIterator<LocatorMap> LocatorMapIterator;
        LocatorMapIterator getLocatorMapIterator(void)
        {
            return LocatorMapIterator(
                mLocatorMap.begin(), mLocatorMap.end());
        }

	protected:

		void _createEntity(void);

		void _setEntityMaterials(void);

		Ogre::Entity* _createEntityImpl( const String& meshName );

		void _destroyEntityImpl(Ogre::Entity* entity);

		void _createMainSceneNode(void);

		void _createLocatorNode(void);

		bool _changeAnimation(const String& animationName, Real delay);

		void _updateLocatorPos(void);

		void _animationBlend(Real delta);

		void _executeLocatorModel(Real delta);

		void _muteCurrentSkillSound(void);

		bool _handleAnimationFinish(Real oldTime, Real currTime);

		void _handleSkillHitTime(Real oldTime, Real currTime);

		void _handleSkillBreakTime(Real oldTime, Real currTime);

        void _handleSkillShakeTime(Real oldTime, Real currTime);

		void _createAnimationEffect(Real oldTime, Real currTime);

		void _updateAttachedAnimationEffect(Real oldTime, Real currTime);

		void _createAnimationRibbon(Real oldTime, Real currTime);

		void _updateAnimationRibbon(Real delta);

		void _updateAnimationSound(Real oldTime, Real currTime);

        void _updateAnimationLight(Real oldTime, Real currTime);

		bool _setEntityMaterial(const String& entityEntry, const String& matName);

		void _createEffects(void);

		void _updateAllEffects(void);

        void _updateModelProjectors(void);

		void _updateBoundingBox(void);

		// ������Ҫ��ת����
		void _rotate(void);

		/// �����ַ��������ò�ͬ���ƶ�����
		void _setMovementType( const String& value );

        /// ɾ������model�ϵ�����projector effect
        void _delAllProjector(void);

        /// ʵ����modelʱ����slot�ϵ�model
        bool _createModelInSlot(SlotValue& slotValue, LocatorValue& locatorValue);

        /// ����Ĭ�϶�������
        void _setDefaultAnimationName(const String& name)
        {
            mDefaultAnimationName = name;
        }

        /// �����������ԣ�û��hint�����ԣ�
        void _dealWithExtraAttributes(const String& name, const String& value);

        /** ��ȡָ�����Ƶ�entity
        @param name Ҫ��ȡ��entity�����ƣ���HairMesh�ȣ�
        */
        Ogre::Entity* _getEntityByName(const String& name);

        /** ������entity
        @remarks ÿ��model����һ����entity����Ϊskeleton entity������entity����master
                 entity����������
                 master entityֻ����LogicModel����ʱ�Żᱻɾ�������Կ��Ա�������������ֵ�
                 entityʱ�����������Ĺ�ϵ����ȷ�ġ�
        @param skeleton ���ڴ���skeleton entity��skeleton
        */
        void _createSkeletonEntity(const Ogre::SkeletonPtr& skeleton);

        /// ɾ��master entity
        void _delSkeletonEntity(void);

        /// ����һ��entity����ɫ
        void _setEntityColour( EntityValue& entityValue, const Ogre::ColourValue& entityColour );
        /// ���һ��entity��clone�Ĳ���
        void _clearEntityMaterials( EntityValue& entityValue );

        void _resetEntityMaterial( EntityValue& entityValue );

        void _updateAllEntitiesTransparency(void);

        /// �����ⲿ��������ݣ�������һ��bounding box
        void _createBoundingBoxFromExternal(void);

        /// �����ⲿ��������ݣ�������������
        void _setScaleFactor(const String& value);

        /// ���ݵ�ǰλ�ã���ȡ����Ӧ��ģ����Ч������
        bool _getTemplateEffectName(String& effectName, const String& templateName, float& height);

        /** ���ݶ������ƣ���ȡAnimationState
        @par �����ǰskeleton entity��û������������Ͷ�ȡ����������ڵĹ����ļ���
             ��link����ǰ��skeleton entity
        */
		Ogre::AnimationState* _getAnimationState(const String& animName);

        /// ���û�ȡ���θ߶ȵĻص�����
        void _getWorldHeight(float x, float z, float& y);

        /// ����ͨ��attachModel����attach��������model
        void _executeAttachedModel(Real delta);

        /// ɾ����������collider������colliderset
        void _destroyColliders(void);

        /// ��execute�и���collider��λ�úͷ�λ
        void _updateFabricColliders(void);

        /// ʵ�������е�collider
        void _initFabricColliders(void);

        /** ����һ��locator node
        @param offsetPos ��ʼƫ����
        @param offsetOri ��ʼƫ����ת��
        */
        Ogre::SceneNode* _createLocatorSceneNode(const Ogre::Vector3& offsetPos, const Ogre::Quaternion& offsetOri);

        /** ��ȡ��locator����Ӧ��scene nodeָ��
        @remarks �����ǰ���locator��scene node��δ�����������ͻ�����������е���_createLocatorSceneNode���д���
        */
        Ogre::SceneNode* _getLocatorSceneNode(LocatorValue& locatorValue);

        /** ���²���Ķ����б�
        @remarks ����Ķ������������һ�𲥷ţ�ֻ����һ�Σ������һ��������������ˣ��Ͱ��������
        �Ӷ����б���ɾ��
        */
        void _updatePlayingAnimations(Real deltaTime);

        /** ���������entity�Ĳ���ת����hardware skinning����
        @remarks ���entity�е�ԭ���ʲ���������Ҫ��ת����ʧ�ܣ���ʹ��ԭ���ʣ�
                ����technique������pass������texture unit
                ����û�й���������entity��Ҳ��ֱ�ӷ���false
        */
        bool _convertEntityToCharacterMaterial(Ogre::Entity* entity);

        Ogre::MaterialPtr _deriveHardwareSkinningMaterial(const Ogre::MaterialPtr& origin,
            const String& prefix, const String& postfix, bool useSpecular);

        /// �жϵ�ǰϵͳ�Ƿ�֧��Ӳ��Ƥ��
        bool _checkForHardwareSkinningMaterialSupport(void);

	protected:

		/// ���ݳ�Ա
		AttribMap mAttribMap;
		EntityMap mEntityMap;
		MaterialMap mMaterialMap;
		LocatorMap mLocatorMap;
		EffectMap mEffectMap;

        ShadowUncastableMeshs mShadowUncastableMeshs;

        FabricInfoMap mFabricInfoMap;

        FabricColliderMap mFabricColliderMap;

        /// ����model�ϵ�����projector effect���п�����ѡ�б�־��������Ӱ��
        ModelProjectors mModelProjectors;

		System* mSystem;

		Ogre::Any mUserAny;

		Ogre::SceneNode* mModelMainNode;
        Ogre::Node* mParentNode;

		Ogre::Entity* mSkeletonEntity;

		ulong mCreatedEntityCount;

		Ogre::SkeletonInstance* mSkeleton;

		//����BoundingBox
		Ogre::AxisAlignedBox* mFullBoundingBox;

		//�Ƿ����BoundingBox
		bool                    mIsPrepareBoundBox;

		Ogre::AnimationState* mCurrentAnimationState;
		Ogre::AnimationState* mFadeOutAnimationState;

		Real mGlobalAnimationRate;

		Real mFadeOutDelay;

		String mCurrentAnimationName;

		bool mAnimationLoop;

        CreatedEffectList mCreatedEffectList;

		uint mEffectHandleCount;

		Skill* mCurrentSkill;

		Real mAnimTimePos;
		bool mAnimEditing;

		String mName;

		bool mChildModel;

		Ogre::Vector3 mModelPosition;
		Ogre::Quaternion mModelOrientation;

		AttachedModelOriginSceneNodes mAttachedModelOriginSceneNodes;

		/// ǰ���ŵص�������Orientation
		Ogre::Quaternion mFrontBackOrientation;
		/// �����ŵص�������Orientation
		Ogre::Quaternion mLeftRightOrientation;

		bool mMainNodeNeedUpdate;

		/// ��dobject�����ͣ���ͬ�������ڸ���ʱ���в�ͬ����Ϊ
		MovementType mMovementType;

        /// ��ǰmodel��͸����
        Real mCurrentTransparency;

        /// Ŀ��͸����
        Real mDestTransparency;

        /// ͸����delta
        Real mDeltaTransparency;

        /// ͸�����Ƿ�Ҫ��ı�
        bool mNeedUpdateTransparency;

        /// ͸���ȹ���ʱ��
        Real mTransparencyTime;

        /// ��ʼ�������ƣ���ʼ���������modelһ���������Ϳ�ʼѭ�����ŵ�
        String mDefaultAnimationName;

        /// ͷ������ɫ
        Ogre::ColourValue mHairColour;
        
        /// �Ƿ����ͶӰ
        bool mShadowCastable;

        /** attach��LogicModel��LogicModel
        @remarks ������attachModel��detachModelʱ��Ҫ��this��ɱ�attach��model��mAttachParent��
                 �������Լ��Ǳ��ĸ�LogicModel��attach��
        */
        LogicModel* mAttachParent;

        /// ��ǰ�Ƿ�ѡ��
        bool mSelected;

        Ogre::AxisAlignedBox* mExternalBoundingBox;
        String mExternalBoundingBoxValue;

        /// �ⲿ�������������
        Ogre::Vector3 mExternalScaleFactor;

        /// ��ײ������
        Ogre::ColliderSet* mColliderSet;

        Real mTerrainHeight;

        uint32 mVisibleFlag;

        bool mEnableWalkingEffect;
        
		// ������ػص�����
		OnAnimationFinish					mOnAnimationFinish;
		OnSkillBreakTime               mOnSkillBreakTime;
		OnSkillHitTime                 mOnSkillHitTime;
        OnSkillShakeTime               mOnSkillShakeTime;
        OnGetEffectName                mOnGetEffectName;

        ulong mCallbackFuncInfo;

        /// �Ƿ�Ϊactive��ֻ��active��model�Ż�ִ��execute
        bool mIsActive;
        /// �Ƿ���Ϊ�ɼ�
        bool mVisible;

		/// ��ȡ��Ϸ����ĸ߶�ֵ����
		/** ��������Ϸ�����У�ĳһ����ĵ���߶�ֵ�����ǵ��εĸ߶�ֵ��Ҳ����
			��������ĸ߶�ֵ����������ĸ߶�ֵ�ڵײ��޷���ȡ�����Ծ�ͨ���ص�
			�������ɸ߲���ݲ�ͬ��������߶�ֵ
 		*/
		static GetHeightInWorld          mGetHeightInWorld;

		// �����ĺ���ָ��
		static OnPlaySound                   mOnPlaySound;
		static OnStopSound                   mOnStopSound;
	};
}




#endif