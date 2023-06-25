#ifndef JNIMAPPER_JNITypes_H_
#define JNIMAPPER_JNITypes_H_

#include <jni.h>

class JNIField;
class JNIMethod;
class JNIClassInstance;

class JNIClass
{
public:
	JNIClass()
	{
	}

	JNIClass(JNIEnv* p_env, jclass class_ptr)
		: p_env(p_env), ptr(class_ptr)
	{
	}

	jclass GetPtr()
	{
		return ptr;
	}

	jobject GetInstance()
	{
		return instance;
	}

	void SetInstance(jobject new_instance)
	{
		instance = new_instance;
	}

	std::unique_ptr<JNIClassInstance> NewInstance(jobject instance)
	{
		return std::move(std::make_unique<JNIClassInstance>(p_env, this, instance));
	}

public:
	std::map<std::string, std::shared_ptr<JNIField>> fields;
	std::map<std::string, std::shared_ptr<JNIMethod>> methods;

protected:
	JNIEnv* p_env;
	jclass ptr;
	jobject instance{ nullptr };
};

////////////////////////////////////////////
// JNIField
////////////////////////////////////////////

class JNIField
{
public:
	JNIField(JNIEnv* p_env, JNIClass* parent, jfieldID field_id, bool is_static)
		: p_env(p_env), parent(parent), id(field_id), is_static(is_static)
	{
	}

	void SetParent(JNIClass* new_parent)
	{
		parent = new_parent;
	}

	JNIClass* GetParent()
	{
		return parent;
	}

	jfieldID GetID()
	{
		return id;
	}

	virtual jint GetValueInt()
	{
		return 0;
	}

	virtual jboolean GetValueBoolean()
	{
		return false;
	}

	virtual jbyte GetValueByte()
	{
		return '\0';
	}

	virtual jchar GetValueChar()
	{
		return '\0';
	}

	virtual jshort GetValueShort()
	{
		return 0;
	}

	virtual jlong GetValueLong()
	{
		return 0;
	}

	virtual jfloat GetValueFloat()
	{
		return 0.f;
	}

	virtual jdouble GetValueDouble()
	{
		return 0;
	}

	virtual jobject GetValueObject()
	{
		return nullptr;
	}

	virtual void SetValueInt(jint new_value)
	{

	}

	virtual void SetValueBoolean(jboolean new_value)
	{

	}

	virtual void SetValueByte(jbyte new_value)
	{

	}

	virtual void SetValueChar(jchar new_value)
	{

	}

	virtual void SetValueShort(jshort new_value)
	{

	}

	virtual void SetValueLong(jlong new_value)
	{

	}

	virtual void SetValueFloat(jfloat new_value)
	{

	}

	virtual void SetValueDouble(jdouble new_value)
	{

	}

	virtual void SetValueObject(jobject new_value)
	{
	}
protected:
	JNIEnv* p_env;
	JNIClass* parent;
	jfieldID id;
	bool is_static;
};

class JNIFieldInt : public JNIField
{
public:
	using JNIField::JNIField;

	jint GetValueInt()
	{
		if (is_static)
			return p_env->GetStaticIntField(parent->GetPtr(), id);

		return p_env->GetIntField(parent->GetInstance(), id);
	}

	void SetValueInt(jint new_value)
	{
		if (is_static)
			return p_env->SetStaticIntField(parent->GetPtr(), id, new_value);

		return p_env->SetIntField(parent->GetInstance(), id, new_value);
	}
};

class JNIFieldBoolean : public JNIField
{
public:
	using JNIField::JNIField;

	jboolean GetValueBoolean()
	{
		if (is_static)
			return p_env->GetStaticBooleanField(parent->GetPtr(), id);

		return p_env->GetBooleanField(parent->GetInstance(), id);
	}

	void SetValueBoolean(jboolean new_value)
	{
		if (is_static)
			return p_env->SetStaticIntField(parent->GetPtr(), id, new_value);

		return p_env->SetIntField(parent->GetInstance(), id, new_value);
	}
};

class JNIFieldByte : public JNIField
{
public:
	using JNIField::JNIField;

	jbyte GetValueByte()
	{
		if (is_static)
			return p_env->GetStaticByteField(parent->GetPtr(), id);

		return p_env->GetByteField(parent->GetInstance(), id);
	}

	void SetValueByte(jchar new_value)
	{
		if (is_static)
			return p_env->SetStaticByteField(parent->GetPtr(), id, new_value);

		return p_env->SetByteField(parent->GetInstance(), id, new_value);
	}
};

class JNIFieldChar : public JNIField
{
public:
	using JNIField::JNIField;

	jchar GetValueChar()
	{
		if (is_static)
			return p_env->GetStaticCharField(parent->GetPtr(), id);

		return p_env->GetCharField(parent->GetInstance(), id);
	}

	void SetValueChar(jchar new_value)
	{
		if (is_static)
			return p_env->SetStaticCharField(parent->GetPtr(), id, new_value);

		return p_env->SetCharField(parent->GetInstance(), id, new_value);
	}
};

class JNIFieldShort : public JNIField
{
public:
	using JNIField::JNIField;

	jshort GetValueShort()
	{
		if (is_static)
			return p_env->GetStaticShortField(parent->GetPtr(), id);

		return p_env->GetShortField(parent->GetInstance(), id);
	}

	void SetValueShort(jshort new_value)
	{
		if (is_static)
			return p_env->SetStaticShortField(parent->GetPtr(), id, new_value);

		return p_env->SetShortField(parent->GetInstance(), id, new_value);
	}
};

class JNIFieldLong : public JNIField
{
public:
	using JNIField::JNIField;

	jlong GetValueLong()
	{
		if (is_static)
			return p_env->GetStaticLongField(parent->GetPtr(), id);

		return p_env->GetLongField(parent->GetInstance(), id);
	}

	void SetValueLong(jlong new_value)
	{
		if (is_static)
			return p_env->SetStaticLongField(parent->GetPtr(), id, new_value);

		return p_env->SetLongField(parent->GetInstance(), id, new_value);
	}
};

class JNIFieldFloat : public JNIField
{
public:
	using JNIField::JNIField;

	jfloat GetValueFloat()
	{
		if (is_static)
			return p_env->GetStaticFloatField(parent->GetPtr(), id);

		return p_env->GetFloatField(parent->GetInstance(), id);
	}

	void SetValueFloat(jfloat new_value)
	{
		if (is_static)
			return p_env->SetStaticFloatField(parent->GetPtr(), id, new_value);

		return p_env->SetFloatField(parent->GetInstance(), id, new_value);
	}
};

class JNIFieldDouble : public JNIField
{
public:
	using JNIField::JNIField;

	jdouble GetValueDouble()
	{
		if (is_static)
			return p_env->GetStaticDoubleField(parent->GetPtr(), id);

		return p_env->GetDoubleField(parent->GetInstance(), id);
	}

	void SetValueDouble(jdouble new_value)
	{
		if (is_static)
			return p_env->SetStaticDoubleField(parent->GetPtr(), id, new_value);

		return p_env->SetDoubleField(parent->GetInstance(), id, new_value);
	}
};

class JNIFieldObject : public JNIField
{
public:
	using JNIField::JNIField;

	jobject GetValueObject()
	{
		if (is_static)
			return p_env->GetStaticObjectField(parent->GetPtr(), id);

		if (parent->GetInstance() == nullptr)
			return nullptr;

		return p_env->GetObjectField(parent->GetInstance(), id);
	}

	void SetValueObject(jobject new_value)
	{
		if (is_static)
			return p_env->SetStaticObjectField(parent->GetPtr(), id, new_value);

		return p_env->SetObjectField(parent->GetInstance(), id, new_value);
	}
};

////////////////////////////////////////////
// JNIMethod
////////////////////////////////////////////

class JNIMethod
{
public:
	JNIMethod(JNIEnv* p_env, JNIClass* parent, jmethodID method_id, bool is_static)
		: p_env(p_env), parent(parent), id(method_id), is_static(is_static)
	{
	}

	void SetParent(JNIClass* new_parent)
	{
		if (this == nullptr)
			return;
		parent = new_parent;
	}

	JNIClass* GetParent()
	{
		return parent;
	}

	jmethodID GetID()
	{
		return id;
	}

	virtual void CallVoid(jvalue* args = nullptr)
	{
	}

	virtual jint CallInt(jvalue* args = nullptr)
	{
		return 0;
	}

	virtual jshort CallShort(jvalue* args = nullptr)
	{
		return 0;
	}

	virtual jlong CallLong(jvalue* args = nullptr)
	{
		return 0;
	}

	virtual jbyte CallByte(jvalue* args = nullptr)
	{
		return '\0';
	}

	virtual jchar CallChar(jvalue* args = nullptr)
	{
		return '\0';
	}

	virtual jboolean CallBoolean(jvalue* args = nullptr)
	{
		return false;
	}

	virtual float CallFloat(jvalue* args = nullptr)
	{
		return 0.f;
	}

	virtual jdouble CallDouble(jvalue* args = nullptr)
	{
		return 0;
	}

	virtual jobject CallObject(jvalue* args = nullptr)
	{
		return nullptr;
	}

protected:
	JNIEnv* p_env;
	JNIClass* parent;
	jmethodID id;
	bool is_static;
};

class JNIMethodVoid : public JNIMethod
{
public:
	using JNIMethod::JNIMethod;

	void CallVoid(jvalue* args = nullptr)
	{
		if (is_static)
			return p_env->CallStaticVoidMethodA(parent->GetPtr(), id, args);

		return p_env->CallVoidMethodA(parent->GetInstance(), id, args);
	}
};

class JNIMethodInt : public JNIMethod
{
public:
	using JNIMethod::JNIMethod;

	jint CallInt(jvalue* args = nullptr)
	{
		if (is_static)
			return p_env->CallStaticIntMethodA(parent->GetPtr(), id, args);

		return p_env->CallIntMethodA(parent->GetInstance(), id, args);
	}
};

class JNIMethodShort : public JNIMethod
{
public:
	using JNIMethod::JNIMethod;

	jshort CallShort(jvalue* args = nullptr)
	{
		if (is_static)
			return p_env->CallStaticShortMethodA(parent->GetPtr(), id, args);

		return p_env->CallShortMethodA(parent->GetInstance(), id, args);
	}
};

class JNIMethodLong : public JNIMethod
{
public:
	using JNIMethod::JNIMethod;

	jlong CallLong(jvalue* args = nullptr)
	{
		if (is_static)
			return p_env->CallStaticLongMethodA(parent->GetPtr(), id, args);

		return p_env->CallLongMethodA(parent->GetInstance(), id, args);
	}
};

class JNIMethodByte : public JNIMethod
{
public:
	using JNIMethod::JNIMethod;

	jbyte CallByte(jvalue* args = nullptr)
	{
		if (is_static)
			return p_env->CallStaticByteMethodA(parent->GetPtr(), id, args);

		return p_env->CallByteMethodA(parent->GetInstance(), id, args);
	}
};

class JNIMethodChar : public JNIMethod
{
public:
	using JNIMethod::JNIMethod;

	jchar CallChar(jvalue* args = nullptr)
	{
		if (is_static)
			return p_env->CallStaticCharMethodA(parent->GetPtr(), id, args);

		return p_env->CallCharMethodA(parent->GetInstance(), id, args);
	}
};

class JNIMethodBoolean : public JNIMethod
{
public:
	using JNIMethod::JNIMethod;

	jboolean CallBoolean(jvalue* args = nullptr)
	{
		if (is_static)
			return p_env->CallStaticBooleanMethodA(parent->GetPtr(), id, args);

		return p_env->CallBooleanMethodA(parent->GetInstance(), id, args);
	}
};

class JNIMethodFloat : public JNIMethod
{
public:
	using JNIMethod::JNIMethod;

	jfloat CallFloat(jvalue* args = nullptr)
	{
		if (is_static)
			return p_env->CallStaticFloatMethodA(parent->GetPtr(), id, args);

		return p_env->CallFloatMethodA(parent->GetInstance(), id, args);
	}
};

class JNIMethodDouble : public JNIMethod
{
public:
	using JNIMethod::JNIMethod;

	jdouble CallDouble(jvalue* args = nullptr)
	{
		if (is_static)
			return p_env->CallStaticDoubleMethodA(parent->GetPtr(), id, args);

		return p_env->CallDoubleMethodA(parent->GetInstance(), id, args);
	}
};

class JNIMethodObject : public JNIMethod
{
public:
	using JNIMethod::JNIMethod;

	jobject CallObject(jvalue* args = nullptr)
	{
		if (is_static)
			return p_env->CallStaticObjectMethodA(parent->GetPtr(), id, args);

		if (parent->GetInstance() == nullptr)
			return nullptr;

		return p_env->CallObjectMethodA(parent->GetInstance(), id, args);
	}
};

class JNIClassInstance : public JNIClass
{
public:
	JNIClassInstance(JNIEnv* p_env, JNIClass* inheriter, jobject class_instance)
	{
		p_env = p_env;
		ptr = inheriter->GetPtr();
		instance = class_instance;
		fields = inheriter->fields;
		methods = inheriter->methods;

		for (auto& field : fields)
		{
			field.second->SetParent(this);
		}

		for (auto& method : methods)
		{
			method.second->SetParent(this);
		}
	}
};


#endif