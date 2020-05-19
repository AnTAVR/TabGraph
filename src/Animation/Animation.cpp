#include "Animation/Animation.hpp"
#include "Debug.hpp"
#include "Mesh/Mesh.hpp"
#include "Node.hpp"
#include "Buffer/BufferHelper.hpp"
#include "Buffer/BufferAccessor.hpp"
#include "Callback.hpp"
#include "Transform.hpp"
#include <SDL2/SDL_timer.h> // for SDL_GetTicks
#include <glm/common.hpp>
#include <glm/gtc/quaternion.hpp>

Animation::Animation() : Object("")
{
	//void (__thiscall Animation::* pFunc)() = &Animation::Play;
	_playCallback = Callback<void()>::Create(std::bind(&Animation::Play, this));
}

std::shared_ptr<Animation> Animation::Create()
{
	return std::shared_ptr<Animation>(new Animation);
}

std::vector<AnimationChannel> Animation::GetChannels() const
{
	return _channels;
}

std::vector<AnimationSampler> Animation::GetSamplers() const
{
	return _samplers;
}

void Animation::AddChannel(AnimationChannel channel)
{
	_channels.push_back(channel);
	_interpolators.resize(_channels.size());
}

void Animation::AddSampler(AnimationSampler sampler)
{
	_samplers.push_back(sampler);
}

void Animation::Reset()
{
	for (auto interpolator : _interpolators) {
		_startTime = SDL_GetTicks();
		interpolator.SetPrevTime(0);
		interpolator.SetNextKey(0);
		interpolator.SetPrevKey(0);
	}
}

void Animation::Advance()
{
	_currentTime = (SDL_GetTicks() - _startTime) / 1000.0;
	bool animationPlayed(false);
	for (auto index = 0u; index < _channels.size(); ++index) {
		auto channel(_channels.at(index));
		auto interpolator(_interpolators.at(index));
		auto sampler(_samplers.at(channel.SamplerIndex()));
		auto t = _currentTime;
		auto max(BufferHelper::Get<float>(sampler.Timings(), sampler.Timings()->Count() - 1));
		auto min(BufferHelper::Get<float>(sampler.Timings(), 0));
		t = std::clamp(t, min, max);
        interpolator.SetPrevTime(t);
        interpolator.SetNextKey(0u);
		for (auto i = interpolator.PrevKey(); i < sampler.Timings()->Count(); ++i)
		{
			float timing(BufferHelper::Get<float>(sampler.Timings(), i));
			if (timing > t)
            {
            	interpolator.SetNextKey(std::clamp(size_t(i), size_t(0), sampler.Timings()->Count() - 1));
                break;
            }
		}
		interpolator.SetPrevKey(std::clamp(size_t(interpolator.NextKey() - 1), size_t(0), size_t(interpolator.NextKey())));
		auto prevTime(BufferHelper::Get<float>(sampler.Timings(), interpolator.PrevKey()));
		auto nextTime(BufferHelper::Get<float>(sampler.Timings(), interpolator.NextKey()));
		auto keyDelta(nextTime - prevTime);
		auto interpolationValue(0.f);
		if (keyDelta != 0)
			interpolationValue = (t - prevTime) / keyDelta;
		switch (channel.Path()) {
			case AnimationChannel::Translation:
			{
				glm::vec3 current = interpolator.Interpolate<glm::vec3>(sampler, keyDelta, interpolationValue);
				channel.Target()->GetTransform()->SetPosition(current);
				break;
			}
			case AnimationChannel::Rotation:
			{
				glm::quat current = interpolator.Interpolate<glm::quat>(sampler, keyDelta, interpolationValue);
				channel.Target()->GetTransform()->SetRotation(glm::normalize(current));
				break;
			}
			case AnimationChannel::Scale:
			{
				glm::vec3 current(interpolator.Interpolate<glm::vec3>(sampler, keyDelta, interpolationValue));
				channel.Target()->GetTransform()->SetScale(current);
				break;
			}
			case AnimationChannel::Weights:
			{
				/*auto mesh(std::dynamic_pointer_cast<Mesh>(channel.Target()));
				if (mesh == nullptr) {
					debugLog(channel.Target()->Name() + " is not a Mesh");
					break;
				}
				auto weights(mesh->Weights());
				if (sampler.Interpolation() == AnimationSampler::CubicSpline)
				{
				}
				else
				{
					for (auto i = 0u; i < weights->Count(); ++i) {
						float prev(BufferHelper::Get<float>(sampler.KeyFrames(), interpolator.PrevKey() + i));
						float next(BufferHelper::Get<float>(sampler.KeyFrames(), nextKey + i));
						auto current = InterpolateKeyFrame(prev, next, interpolationValue, sampler.Interpolation());
						BufferHelper::Set(weights, i, current);
					}
				}*/
				break;
			}
			case AnimationChannel::None:
				break;
		}
		animationPlayed |= t < max;
	}
	if (!animationPlayed) {
		if (Repeat())
			Reset();
		else
			Stop();
	}
}

void Animation::Play()
{
	if (!Playing()) {
		for (auto sampler : _samplers) {
			sampler.Timings()->GetBufferView()->GetBuffer()->LoadToCPU();
			sampler.KeyFrames()->GetBufferView()->GetBuffer()->LoadToCPU();
		}
		_startTime = SDL_GetTicks();
		_playing = true;
	}
}

void Animation::Stop()
{
	Reset();
	_playing = false;
}

bool Animation::Playing() const
{
	return _playing;
}

void Animation::SetRepeat(bool repeat)
{
	_repeat = repeat;
}

bool Animation::Repeat() const
{
	return _repeat;
}
