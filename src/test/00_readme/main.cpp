#include <UDRefl/UDRefl.h>

#include <iostream>

using namespace Ubpa::UDRefl;

struct Vec {
	float x;
	float y;
	float norm() const noexcept {
		return std::sqrt(x * x + y * y);
	}
};

int main() {
	TypeID ID_Vec   = ReflMngr::Instance().tregistry.Register("Vec");
	TypeID ID_float = ReflMngr::Instance().tregistry.Register("float");

	NameID ID_x = ReflMngr::Instance().nregistry.Register("x");
	NameID ID_y = ReflMngr::Instance().nregistry.Register("y");

	NameID ID_norm = ReflMngr::Instance().nregistry.Register("norm");

	NameID ID_ctor = ReflMngr::Instance().nregistry.GetID(NameRegistry::Meta::ctor); // meta function
	NameID ID_dtor = ReflMngr::Instance().nregistry.GetID(NameRegistry::Meta::dtor); // meta function

	{ // register Vec
		ReflMngr::Instance().typeinfos[ID_Vec] = {
			sizeof(Vec),  // size
			alignof(Vec), // alignment
			{ // field infos
				{ ID_x, { { ID_float, offsetof(Vec, x) } }}, // x
				{ ID_y, { { ID_float, offsetof(Vec, y) } }}  // y
			},
			{ // method infos
				{ID_ctor, {MethodPtr::GenerateDefaultConstructor<Vec>()}}, // default ctor
				{ID_dtor, {MethodPtr::GenerateDestructor<Vec>()}},         // dtor
				{ID_norm, {{                                               // norm
					[](const void* obj, ArgsView, void* result_buffer) -> Destructor* {
						buffer_get<float>(result_buffer, 0) = buffer_as<Vec>(obj).norm();
						return nullptr;
					}
				}}}
			}
		};
	}

	ObjectPtr v = ReflMngr::Instance().New(ID_Vec);

	ReflMngr::Instance().RWVar(v, ID_x).As<float>() = 3.f;
	ReflMngr::Instance().RWVar(v, ID_y).As<float>() = 4.f;

	std::cout << "x: " << ReflMngr::Instance().RVar(v, ID_x).As<float>() << std::endl;
	std::cout << "norm: " << ReflMngr::Instance().Invoke<float>(v, ID_norm) << std::endl;
	
	ReflMngr::Instance().ForEachField(
		ID_Vec, // TypeID
		[](Type type, Field field) {
			std::cout << ReflMngr::Instance().nregistry.Nameof(field.ID) << std::endl;
		}
	);

	ReflMngr::Instance().ForEachMethod(
		ID_Vec, // TypeID
		[](Type type, Method method) {
			std::cout << ReflMngr::Instance().nregistry.Nameof(method.ID) << std::endl;
		}
	);

	ReflMngr::Instance().ForEachRVar(
		v, // ObjectPtr
		[](Type type, Field field, ConstObjectPtr var) {
			std::cout
				<< ReflMngr::Instance().nregistry.Nameof(field.ID)
				<< ": " << var.As<float>()
				<< std::endl;
		}
	);

	ReflMngr::Instance().Delete(v);
}