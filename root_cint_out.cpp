// Do NOT change. Changes will be lost next time file is generated

#define R__DICTIONARY_FILENAME dIhomedIwangyingdIworkdImisdIothersdIADASdImuon150_anadIroot_cint_out
#define R__NO_DEPRECATION

/*******************************************************************/
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#define G__DICTIONARY
#include "ROOT/RConfig.hxx"
#include "TClass.h"
#include "TDictAttributeMap.h"
#include "TInterpreter.h"
#include "TROOT.h"
#include "TBuffer.h"
#include "TMemberInspector.h"
#include "TInterpreter.h"
#include "TVirtualMutex.h"
#include "TError.h"

#ifndef G__ROOT
#define G__ROOT
#endif

#include "RtypesImp.h"
#include "TIsAProxy.h"
#include "TFileMergeInfo.h"
#include <algorithm>
#include "TCollectionProxyInfo.h"
/*******************************************************************/

#include "TDataMember.h"

// Header files passed as explicit arguments
#include "data_strcut_cint.h"

// Header files passed via #pragma extra_include

// The generated code does not explicitly qualify STL entities
namespace std {} using namespace std;

namespace ROOT {
   static void *new_cluster(void *p = nullptr);
   static void *newArray_cluster(Long_t size, void *p);
   static void delete_cluster(void *p);
   static void deleteArray_cluster(void *p);
   static void destruct_cluster(void *p);
   static void streamer_cluster(TBuffer &buf, void *obj);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::cluster*)
   {
      ::cluster *ptr = nullptr;
      static ::TVirtualIsAProxy* isa_proxy = new ::TInstrumentedIsAProxy< ::cluster >(nullptr);
      static ::ROOT::TGenericClassInfo 
         instance("cluster", ::cluster::Class_Version(), "data_strcut_cint.h", 11,
                  typeid(::cluster), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &::cluster::Dictionary, isa_proxy, 16,
                  sizeof(::cluster) );
      instance.SetNew(&new_cluster);
      instance.SetNewArray(&newArray_cluster);
      instance.SetDelete(&delete_cluster);
      instance.SetDeleteArray(&deleteArray_cluster);
      instance.SetDestructor(&destruct_cluster);
      instance.SetStreamerFunc(&streamer_cluster);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::cluster*)
   {
      return GenerateInitInstanceLocal(static_cast<::cluster*>(nullptr));
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal(static_cast<const ::cluster*>(nullptr)); R__UseDummy(_R__UNIQUE_DICT_(Init));
} // end of namespace ROOT

namespace ROOT {
   static void delete_hit_position(void *p);
   static void deleteArray_hit_position(void *p);
   static void destruct_hit_position(void *p);
   static void streamer_hit_position(TBuffer &buf, void *obj);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::hit_position*)
   {
      ::hit_position *ptr = nullptr;
      static ::TVirtualIsAProxy* isa_proxy = new ::TInstrumentedIsAProxy< ::hit_position >(nullptr);
      static ::ROOT::TGenericClassInfo 
         instance("hit_position", ::hit_position::Class_Version(), "data_strcut_cint.h", 31,
                  typeid(::hit_position), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &::hit_position::Dictionary, isa_proxy, 16,
                  sizeof(::hit_position) );
      instance.SetDelete(&delete_hit_position);
      instance.SetDeleteArray(&deleteArray_hit_position);
      instance.SetDestructor(&destruct_hit_position);
      instance.SetStreamerFunc(&streamer_hit_position);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::hit_position*)
   {
      return GenerateInitInstanceLocal(static_cast<::hit_position*>(nullptr));
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal(static_cast<const ::hit_position*>(nullptr)); R__UseDummy(_R__UNIQUE_DICT_(Init));
} // end of namespace ROOT

//______________________________________________________________________________
atomic_TClass_ptr cluster::fgIsA(nullptr);  // static to hold class pointer

//______________________________________________________________________________
const char *cluster::Class_Name()
{
   return "cluster";
}

//______________________________________________________________________________
const char *cluster::ImplFileName()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::cluster*)nullptr)->GetImplFileName();
}

//______________________________________________________________________________
int cluster::ImplFileLine()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::cluster*)nullptr)->GetImplFileLine();
}

//______________________________________________________________________________
TClass *cluster::Dictionary()
{
   fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::cluster*)nullptr)->GetClass();
   return fgIsA;
}

//______________________________________________________________________________
TClass *cluster::Class()
{
   if (!fgIsA.load()) { R__LOCKGUARD(gInterpreterMutex); fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::cluster*)nullptr)->GetClass(); }
   return fgIsA;
}

//______________________________________________________________________________
atomic_TClass_ptr hit_position::fgIsA(nullptr);  // static to hold class pointer

//______________________________________________________________________________
const char *hit_position::Class_Name()
{
   return "hit_position";
}

//______________________________________________________________________________
const char *hit_position::ImplFileName()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::hit_position*)nullptr)->GetImplFileName();
}

//______________________________________________________________________________
int hit_position::ImplFileLine()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::hit_position*)nullptr)->GetImplFileLine();
}

//______________________________________________________________________________
TClass *hit_position::Dictionary()
{
   fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::hit_position*)nullptr)->GetClass();
   return fgIsA;
}

//______________________________________________________________________________
TClass *hit_position::Class()
{
   if (!fgIsA.load()) { R__LOCKGUARD(gInterpreterMutex); fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::hit_position*)nullptr)->GetClass(); }
   return fgIsA;
}

//______________________________________________________________________________
void cluster::Streamer(TBuffer &R__b)
{
   // Stream an object of class cluster.

   UInt_t R__s, R__c;
   if (R__b.IsReading()) {
      Version_t R__v = R__b.ReadVersion(&R__s, &R__c); if (R__v) { }
      TObject::Streamer(R__b);
      R__b.StreamObject(&(range),typeid(range));
      {
         vector<int> &R__stl =  adc_amp;
         R__stl.clear();
         int R__i, R__n;
         R__b >> R__n;
         R__stl.reserve(R__n);
         for (R__i = 0; R__i < R__n; R__i++) {
            int R__t;
            R__b >> R__t;
            R__stl.push_back(R__t);
         }
      }
      R__b >> holes;
      R__b >> strips;
      R__b.CheckByteCount(R__s, R__c, cluster::IsA());
   } else {
      R__c = R__b.WriteVersion(cluster::IsA(), kTRUE);
      TObject::Streamer(R__b);
      R__b.StreamObject(&(range),typeid(range));
      {
         vector<int> &R__stl =  adc_amp;
         int R__n=int(R__stl.size());
         R__b << R__n;
         if(R__n) {
            vector<int>::iterator R__k;
            for (R__k = R__stl.begin(); R__k != R__stl.end(); ++R__k) {
            R__b << (*R__k);
            }
         }
      }
      R__b << holes;
      R__b << strips;
      R__b.SetByteCount(R__c, kTRUE);
   }
}

namespace ROOT {
   // Wrappers around operator new
   static void *new_cluster(void *p) {
      return  p ? new(p) ::cluster : new ::cluster;
   }
   static void *newArray_cluster(Long_t nElements, void *p) {
      return p ? new(p) ::cluster[nElements] : new ::cluster[nElements];
   }
   // Wrapper around operator delete
   static void delete_cluster(void *p) {
      delete (static_cast<::cluster*>(p));
   }
   static void deleteArray_cluster(void *p) {
      delete [] (static_cast<::cluster*>(p));
   }
   static void destruct_cluster(void *p) {
      typedef ::cluster current_t;
      (static_cast<current_t*>(p))->~current_t();
   }
   // Wrapper around a custom streamer member function.
   static void streamer_cluster(TBuffer &buf, void *obj) {
      ((::cluster*)obj)->::cluster::Streamer(buf);
   }
} // end of namespace ROOT for class ::cluster

//______________________________________________________________________________
void hit_position::Streamer(TBuffer &R__b)
{
   // Stream an object of class hit_position.

   UInt_t R__s, R__c;
   if (R__b.IsReading()) {
      Version_t R__v = R__b.ReadVersion(&R__s, &R__c); if (R__v) { }
      TObject::Streamer(R__b);
      R__b >> layer_id;
      {
         vector<double> &R__stl =  position_x;
         R__stl.clear();
         int R__i, R__n;
         R__b >> R__n;
         R__stl.reserve(R__n);
         for (R__i = 0; R__i < R__n; R__i++) {
            double R__t;
            R__b >> R__t;
            R__stl.push_back(R__t);
         }
      }
      {
         vector<double> &R__stl =  position_y;
         R__stl.clear();
         int R__i, R__n;
         R__b >> R__n;
         R__stl.reserve(R__n);
         for (R__i = 0; R__i < R__n; R__i++) {
            double R__t;
            R__b >> R__t;
            R__stl.push_back(R__t);
         }
      }
      R__b.CheckByteCount(R__s, R__c, hit_position::IsA());
   } else {
      R__c = R__b.WriteVersion(hit_position::IsA(), kTRUE);
      TObject::Streamer(R__b);
      R__b << layer_id;
      {
         vector<double> &R__stl =  position_x;
         int R__n=int(R__stl.size());
         R__b << R__n;
         if(R__n) {
            vector<double>::iterator R__k;
            for (R__k = R__stl.begin(); R__k != R__stl.end(); ++R__k) {
            R__b << (*R__k);
            }
         }
      }
      {
         vector<double> &R__stl =  position_y;
         int R__n=int(R__stl.size());
         R__b << R__n;
         if(R__n) {
            vector<double>::iterator R__k;
            for (R__k = R__stl.begin(); R__k != R__stl.end(); ++R__k) {
            R__b << (*R__k);
            }
         }
      }
      R__b.SetByteCount(R__c, kTRUE);
   }
}

namespace ROOT {
   // Wrapper around operator delete
   static void delete_hit_position(void *p) {
      delete (static_cast<::hit_position*>(p));
   }
   static void deleteArray_hit_position(void *p) {
      delete [] (static_cast<::hit_position*>(p));
   }
   static void destruct_hit_position(void *p) {
      typedef ::hit_position current_t;
      (static_cast<current_t*>(p))->~current_t();
   }
   // Wrapper around a custom streamer member function.
   static void streamer_hit_position(TBuffer &buf, void *obj) {
      ((::hit_position*)obj)->::hit_position::Streamer(buf);
   }
} // end of namespace ROOT for class ::hit_position

namespace ROOT {
   static TClass *vectorlEintgR_Dictionary();
   static void vectorlEintgR_TClassManip(TClass*);
   static void *new_vectorlEintgR(void *p = nullptr);
   static void *newArray_vectorlEintgR(Long_t size, void *p);
   static void delete_vectorlEintgR(void *p);
   static void deleteArray_vectorlEintgR(void *p);
   static void destruct_vectorlEintgR(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const vector<int>*)
   {
      vector<int> *ptr = nullptr;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(vector<int>));
      static ::ROOT::TGenericClassInfo 
         instance("vector<int>", -2, "vector", 389,
                  typeid(vector<int>), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &vectorlEintgR_Dictionary, isa_proxy, 0,
                  sizeof(vector<int>) );
      instance.SetNew(&new_vectorlEintgR);
      instance.SetNewArray(&newArray_vectorlEintgR);
      instance.SetDelete(&delete_vectorlEintgR);
      instance.SetDeleteArray(&deleteArray_vectorlEintgR);
      instance.SetDestructor(&destruct_vectorlEintgR);
      instance.AdoptCollectionProxyInfo(TCollectionProxyInfo::Generate(TCollectionProxyInfo::Pushback< vector<int> >()));

      instance.AdoptAlternate(::ROOT::AddClassAlternate("vector<int>","std::vector<int, std::allocator<int> >"));
      return &instance;
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal(static_cast<const vector<int>*>(nullptr)); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *vectorlEintgR_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal(static_cast<const vector<int>*>(nullptr))->GetClass();
      vectorlEintgR_TClassManip(theClass);
   return theClass;
   }

   static void vectorlEintgR_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrappers around operator new
   static void *new_vectorlEintgR(void *p) {
      return  p ? ::new(static_cast<::ROOT::Internal::TOperatorNewHelper*>(p)) vector<int> : new vector<int>;
   }
   static void *newArray_vectorlEintgR(Long_t nElements, void *p) {
      return p ? ::new(static_cast<::ROOT::Internal::TOperatorNewHelper*>(p)) vector<int>[nElements] : new vector<int>[nElements];
   }
   // Wrapper around operator delete
   static void delete_vectorlEintgR(void *p) {
      delete (static_cast<vector<int>*>(p));
   }
   static void deleteArray_vectorlEintgR(void *p) {
      delete [] (static_cast<vector<int>*>(p));
   }
   static void destruct_vectorlEintgR(void *p) {
      typedef vector<int> current_t;
      (static_cast<current_t*>(p))->~current_t();
   }
} // end of namespace ROOT for class vector<int>

namespace ROOT {
   static TClass *vectorlEdoublegR_Dictionary();
   static void vectorlEdoublegR_TClassManip(TClass*);
   static void *new_vectorlEdoublegR(void *p = nullptr);
   static void *newArray_vectorlEdoublegR(Long_t size, void *p);
   static void delete_vectorlEdoublegR(void *p);
   static void deleteArray_vectorlEdoublegR(void *p);
   static void destruct_vectorlEdoublegR(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const vector<double>*)
   {
      vector<double> *ptr = nullptr;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(vector<double>));
      static ::ROOT::TGenericClassInfo 
         instance("vector<double>", -2, "vector", 389,
                  typeid(vector<double>), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &vectorlEdoublegR_Dictionary, isa_proxy, 0,
                  sizeof(vector<double>) );
      instance.SetNew(&new_vectorlEdoublegR);
      instance.SetNewArray(&newArray_vectorlEdoublegR);
      instance.SetDelete(&delete_vectorlEdoublegR);
      instance.SetDeleteArray(&deleteArray_vectorlEdoublegR);
      instance.SetDestructor(&destruct_vectorlEdoublegR);
      instance.AdoptCollectionProxyInfo(TCollectionProxyInfo::Generate(TCollectionProxyInfo::Pushback< vector<double> >()));

      instance.AdoptAlternate(::ROOT::AddClassAlternate("vector<double>","std::vector<double, std::allocator<double> >"));
      return &instance;
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal(static_cast<const vector<double>*>(nullptr)); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *vectorlEdoublegR_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal(static_cast<const vector<double>*>(nullptr))->GetClass();
      vectorlEdoublegR_TClassManip(theClass);
   return theClass;
   }

   static void vectorlEdoublegR_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrappers around operator new
   static void *new_vectorlEdoublegR(void *p) {
      return  p ? ::new(static_cast<::ROOT::Internal::TOperatorNewHelper*>(p)) vector<double> : new vector<double>;
   }
   static void *newArray_vectorlEdoublegR(Long_t nElements, void *p) {
      return p ? ::new(static_cast<::ROOT::Internal::TOperatorNewHelper*>(p)) vector<double>[nElements] : new vector<double>[nElements];
   }
   // Wrapper around operator delete
   static void delete_vectorlEdoublegR(void *p) {
      delete (static_cast<vector<double>*>(p));
   }
   static void deleteArray_vectorlEdoublegR(void *p) {
      delete [] (static_cast<vector<double>*>(p));
   }
   static void destruct_vectorlEdoublegR(void *p) {
      typedef vector<double> current_t;
      (static_cast<current_t*>(p))->~current_t();
   }
} // end of namespace ROOT for class vector<double>

namespace {
  void TriggerDictionaryInitialization_root_cint_out_Impl() {
    static const char* headers[] = {
"data_strcut_cint.h",
nullptr
    };
    static const char* includePaths[] = {
"/home/wangying/work/mis/others/ADAS/muon150_ana",
"/home/wangying/software/root-6.30.04-build/include/",
"/home/wangying/work/mis/others/ADAS/muon150_ana/build/",
nullptr
    };
    static const char* fwdDeclCode = R"DICTFWDDCLS(
#line 1 "root_cint_out dictionary forward declarations' payload"
#pragma clang diagnostic ignored "-Wkeyword-compat"
#pragma clang diagnostic ignored "-Wignored-attributes"
#pragma clang diagnostic ignored "-Wreturn-type-c-linkage"
extern int __Cling_AutoLoading_Map;
struct __attribute__((annotate("$clingAutoload$data_strcut_cint.h")))  cluster;
struct __attribute__((annotate("$clingAutoload$data_strcut_cint.h")))  hit_position;
)DICTFWDDCLS";
    static const char* payloadCode = R"DICTPAYLOAD(
#line 1 "root_cint_out dictionary payload"


#define _BACKWARD_BACKWARD_WARNING_H
// Inline headers
#include "data_strcut_cint.h"

#undef  _BACKWARD_BACKWARD_WARNING_H
)DICTPAYLOAD";
    static const char* classesHeaders[] = {
"cluster", payloadCode, "@",
"hit_position", payloadCode, "@",
nullptr
};
    static bool isInitialized = false;
    if (!isInitialized) {
      TROOT::RegisterModule("root_cint_out",
        headers, includePaths, payloadCode, fwdDeclCode,
        TriggerDictionaryInitialization_root_cint_out_Impl, {}, classesHeaders, /*hasCxxModule*/false);
      isInitialized = true;
    }
  }
  static struct DictInit {
    DictInit() {
      TriggerDictionaryInitialization_root_cint_out_Impl();
    }
  } __TheDictionaryInitializer;
}
void TriggerDictionaryInitialization_root_cint_out() {
  TriggerDictionaryInitialization_root_cint_out_Impl();
}
