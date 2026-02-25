#ifndef COMPONENT_H
#define COMPONENT_H

namespace URay
{
	class Unit;

	class Component
	{
	public:
		virtual ~Component() = default;

	public:
		virtual void Initialize() = 0;
		virtual void Update() = 0;
		virtual void Render() = 0;
		virtual void Finalize() = 0;

		void SetOwner(Unit* owner) { _owner = owner; }
		Unit* GetOwner() const { return _owner; }

	private:
		Unit* _owner = nullptr;
	};
}

#endif
