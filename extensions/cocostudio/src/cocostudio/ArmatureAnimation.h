/****************************************************************************
Copyright (c) 2013-2017 Chukong Technologies Inc.

https://axmol.dev/

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
****************************************************************************/

#ifndef __CCANIMATION_H__
#define __CCANIMATION_H__

#include "ProcessBase.h"
#include "Tween.h"
#include "CocosStudioExport.h"
#include <queue>

namespace cocostudio
{

enum MovementEventType
{
    START,
    COMPLETE,
    LOOP_COMPLETE
};

class Armature;
class Bone;

typedef void (ax::Object::*SEL_MovementEventCallFunc)(Armature*, MovementEventType, std::string_view);
typedef void (ax::Object::*SEL_FrameEventCallFunc)(Bone*, std::string_view, int, int);

#define movementEvent_selector(_SELECTOR) (cocostudio::SEL_MovementEventCallFunc)(&_SELECTOR)
#define frameEvent_selector(_SELECTOR) (cocostudio::SEL_FrameEventCallFunc)(&_SELECTOR)

struct FrameEvent
{
    Bone* bone;
    std::string frameEventName;
    int originFrameIndex;
    int currentFrameIndex;
};

struct MovementEvent
{
    Armature* armature;
    MovementEventType movementType;
    std::string movementID;
};

class CCS_DLL ArmatureAnimation : public ProcessBase
{
public:
    /**
     * Create with a Armature
     * @param armature The Armature ArmatureAnimation will bind to
     */
    static ArmatureAnimation* create(Armature* armature);

public:
    /**
     * @js ctor
     */
    ArmatureAnimation();
    /**
     * @js NA
     * @lua NA
     */
    virtual ~ArmatureAnimation(void);

    /**
     * Init with a Armature
     * @param armature The Armature ArmatureAnimation will bind to
     */
    virtual bool init(Armature* armature);

    /**
     * Scale animation play speed.
     * This method is deprecated, please use setSpeedScale.
     * @param animationScale Scale value
     */
    AX_DEPRECATED_ATTRIBUTE virtual void setAnimationScale(float animationScale);
    AX_DEPRECATED_ATTRIBUTE virtual float getAnimationScale() const;

    /**
     * Scale animation play speed.
     * @param animationScale Scale value
     */
    virtual void setSpeedScale(float speedScale);
    virtual float getSpeedScale() const;

    //! The animation update speed
    AX_DEPRECATED_ATTRIBUTE virtual void setAnimationInternal(float animationInternal) {}

    using ProcessBase::play;
    /**
     * Play animation by animation name.
     *
     * @param  animationName  The animation name you want to play
     * @param  durationTo The frames between two animation changing-over.
     *         It's meaning is changing to this animation need how many frames
     *
     *         -1 : use the value from MovementData get from flash design panel
     * @param  loop   Whether the animation is loop
     *
     *         loop < 0 : use the value from MovementData get from flash design panel
     *         loop = 0 : this animation is not loop
     *         loop > 0 : this animation is loop
     */
    virtual void play(std::string_view animationName, int durationTo = -1, int loop = -1);

    /**
     * Play animation by index, the other param is the same to play.
     * @deprecated, please use playWithIndex
     * @param  animationIndex  the animation index you want to play
     */
    AX_DEPRECATED_ATTRIBUTE virtual void playByIndex(int animationIndex, int durationTo = -1, int loop = -1);
    virtual void playWithIndex(int animationIndex, int durationTo = -1, int loop = -1);

    virtual void playWithNames(const std::vector<std::string>& movementNames, int durationTo = -1, bool loop = true);
    virtual void playWithIndexes(const std::vector<int>& movementIndexes, int durationTo = -1, bool loop = true);

    /**
     * Go to specified frame and play current movement.
     * You need first switch to the movement you want to play, then call this function.
     *
     * example : playByIndex(0);
     *           gotoAndPlay(0);
     *           playByIndex(1);
     *           gotoAndPlay(0);
     *           gotoAndPlay(15);
     */
    virtual void gotoAndPlay(int frameIndex);

    /**
     * Go to specified frame and pause current movement.
     */
    virtual void gotoAndPause(int frameIndex);

    /**
     * Pause the Process
     */
    virtual void pause() override;
    /**
     * Resume the Process
     */
    virtual void resume() override;
    /**
     * Stop the Process
     */
    virtual void stop() override;

    /**
     * Get movement count
     */
    ssize_t getMovementCount() const;

    virtual void update(float dt) override;

    /**
     * Get current movementID
     * @return The name of current movement
     */
    std::string getCurrentMovementID() const;

    /**
     * Set armature's movement event callback function
     * To disconnect this event, just setMovementEventCallFunc(nullptr, nullptr);
     */
    AX_DEPRECATED_ATTRIBUTE void setMovementEventCallFunc(ax::Object* target, SEL_MovementEventCallFunc callFunc);

    /**
     * Set armature's frame event callback function
     * To disconnect this event, just setFrameEventCallFunc(nullptr, nullptr);
     */
    AX_DEPRECATED_ATTRIBUTE void setFrameEventCallFunc(ax::Object* target, SEL_FrameEventCallFunc callFunc);

    void setMovementEventCallFunc(
        std::function<void(Armature* armature, MovementEventType movementType, std::string_view movementID)> listener);
    void setFrameEventCallFunc(
        std::function<void(Bone* bone, std::string_view frameEventName, int originFrameIndex, int currentFrameIndex)>
            listener);

    virtual void setAnimationData(AnimationData* data)
    {
        if (_animationData != data)
        {
            AX_SAFE_RETAIN(data);
            AX_SAFE_RELEASE(_animationData);
            _animationData = data;
        }
    }
    virtual AnimationData* getAnimationData() const { return _animationData; }

    /**
     * Returns a user assigned Object
     *
     * Similar to userData, but instead of holding a void* it holds an object
     *
     * @return A user assigned Object
     * @js NA
     * @lua NA
     */
    virtual Object* getUserObject() { return _userObject; }
    /**
     * @js NA
     * @lua NA
     */
    virtual const Object* getUserObject() const { return _userObject; }

    /**
     * Returns a user assigned Object
     *
     * Similar to UserData, but instead of holding a void* it holds an object.
     * The UserObject will be retained once in this method,
     * and the previous UserObject (if existed) will be release.
     * The UserObject will be released in Node's destructor.
     *
     * @param userObject    A user assigned Object
     */
    virtual void setUserObject(Object* userObject);

protected:
    /**
     * Update(float dt) will call this handler, you can handle your logic here
     * @js NA
     * @lua NA
     */
    void updateHandler() override;

    /**
     * Update current key frame, and process auto stop, pause
     * @js NA
     * @lua NA
     */
    void updateFrameData(float currentPercent);

    /**
     * Emit a frame event
     * @js NA
     * @lua NA
     */
    void frameEvent(Bone* bone, std::string_view frameEventName, int originFrameIndex, int currentFrameIndex);

    /**
     * Emit a movement event
     */
    void movementEvent(Armature* armature, MovementEventType movementType, std::string_view movementID);

    void updateMovementList();

    bool isIgnoreFrameEvent() const { return _ignoreFrameEvent; }

    friend class Tween;

protected:
    //! AnimationData save all MovementDatas this animation used.
    AnimationData* _animationData;

    //! Scale the animation speed
    float _speedScale;

    MovementData* _movementData;  //! MovementData save all MovementFrameDatas this animation used.

    Armature* _armature;  //! A weak reference of armature

    std::string _movementID;  //! Current movement's name

    int _toIndex;  //! The frame index in MovementData->m_pMovFrameDataArr, it's different from m_iFrameIndex.

    std::vector<Tween*> _tweenList;

    bool _ignoreFrameEvent;

    std::queue<FrameEvent*> _frameEventQueue;
    std::queue<MovementEvent*> _movementEventQueue;

    std::vector<std::string> _movementList;

    bool _onMovementList;
    bool _movementListLoop;
    unsigned int _movementIndex;
    int _movementListDurationTo;

    ax::Object* _userObject;

protected:
    /**
     * MovementEvent CallFunc.
     * @param Armature* a Armature
     * @param MovementEventType, Event Type, like START, COMPLETE.
     * @param const char*, Movement ID, also called Movement Name
     */
    SEL_MovementEventCallFunc _movementEventCallFunc;

    /**
     * FrameEvent CallFunc.
     * @param Bone*, a Bone
     * @param const char*, the name of this frame event
     * @param int, origin frame index
     * @param int, current frame index, animation may be delayed
     */
    SEL_FrameEventCallFunc _frameEventCallFunc;

    ax::Object* _movementEventTarget;
    ax::Object* _frameEventTarget;

    std::function<void(Armature* armature, MovementEventType movementType, std::string_view movementID)>
        _movementEventListener;
    std::function<void(Bone* bone, std::string_view frameEventName, int originFrameIndex, int currentFrameIndex)>
        _frameEventListener;
};

}  // namespace cocostudio

#endif /*__CCANIMATION_H__*/
