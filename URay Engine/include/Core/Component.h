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
		virtual void Initialize() {}
		virtual void Update() {}
		virtual void Render() {}
		virtual void Finalize() {}

		void SetOwner(Unit* owner) { _owner = owner; }
		Unit* GetOwner() const { return _owner; }

	private:
		Unit* _owner = nullptr;
	};
}

#endif
