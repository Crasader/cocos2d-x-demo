/*


#include <vld.h>
#include "CxVar.h"

using namespace std;


void test_1()
{
	int n = 0;
	size_t k = sizeof(CxVar);
	k = sizeof(int64_t);

	CxVar var;

	var.SetValue<int, V_TYPE_I32>(5);

	int r = var.GetValue<int>(n);
	assert(n == 5);


	float f = 0.2f;
	var.SetValue<float, V_TYPE_FLOAT>(0.3f);
	r = var.GetValue<float>(f);
	assert(f == 0.3f);

	double d = 0.32;
	var.SetValue<double,V_TYPE_DOUBLE>(0.55);
	r = var.GetValue<double>(d);
	assert(d == 0.55);


	var.SetString("1234567890", 5);
	string str;
	var.GetString(str);
	assert(0 == str.compare("12345"));

	//blob
	var.SetBlob("1234567890", 5);
	char* c = var.GetData();
	assert(strcmp(c,"12345")==0);

}

void test_2()
{
	CxArrayByte ar;
	CxVar var;
	size_t p = 0;

	var.SetValue<int, V_TYPE_I32>(-5);
	var.WriteToArrayByte(ar);

	CxVar v1;
	p = v1.LoadFromByte(&ar[0]);
	int n = 0;
	int r = v1.GetValue<int>(n);
	assert(n == -5);


	ar.clear();
	var.SetValue<float, V_TYPE_FLOAT>(0.3f);
	var.WriteToArrayByte(ar);

	size_t k = ar.size();
	p = v1.LoadFromByte(&ar[0]);
	float f = 0.0f;
	r = v1.GetValue<float>(f);
	assert(f == 0.3f);

	ar.clear();


	//64
	var.SetValue<int, V_TYPE_U64>(15);
	var.WriteToArrayByte(ar);

	p = v1.LoadFromByte(&ar[0]);
	uint64 _u64 = 0;
	r = v1.GetValue<uint64>(_u64);
	assert(_u64 == 15);


	//string
	ar.clear();
	var.SetString("12345677890", 6);
	var.WriteToArrayByte(ar);
	p = v1.LoadFromByte(&ar[0]);
	string str;
	r = v1.GetString(str);
	assert(0 == str.compare("123456"));

}

void test_v()
{
	CxArrayByte ar;
	CxVar var;
	size_t p = 0;

	var.SetValue<int,V_TYPE_I32>(5);
	var.WriteToArrayByte(ar);

	CxVar v1;
	p = v1.LoadFromByte(&ar[0]);
	int n = 0;
	int r= v1.GetValue<int>(n);
	assert(n == 5);

	CxVarVector* vv = var.SetVector();

//	var.VectorPushBack<int, V_TYPE_I32>(5);

	CxVar::VectorPushBack<int, V_TYPE_I32>(*vv,6);
	CxVar::VectorPushBack<int, V_TYPE_I32>(*vv, 13);

	ar.clear();
	var.WriteToArrayByte(ar);

	size_t k = ar.size();
	var.Clear();
	var.LoadFromByte(&ar[0]);
	vv = var.GetVector();
	assert(vv);

	CxVar* v11 = vv->at(0);
	r = v11->GetValue<int>(n);
	assert(n == 6);

}

void test_m()
{
	int n, r;
	CxArrayByte ar;
	CxVar var;
	CxVarMap* vm = var.SetMap();

	CxVar::MapAdd<int, V_TYPE_I32>(*vm, "age", 18);
	CxVar::MapAdd<int, V_TYPE_I32>(*vm, "sex", 1);
	var.WriteToArrayByte(ar);

	var.Clear();
	var.LoadFromByte(&ar[0]);
	assert(V_TYPE_MAP == var.GetType());

	CxVar* v1 = (*vm)["age"];

	r = v1->GetValue<int>(n);
	assert(n == 18);

	v1 = (*vm)["sex"];
	r = v1->GetValue<int>(n);
	assert(n == 1);

}


int main()
{
	int64 k = 2;
	int64* j = &k;
	test_1();
	test_2();

	test_v();
	test_m();

	return 0;
}




*/