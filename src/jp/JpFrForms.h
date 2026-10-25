// JP-only Fresh additions (no KR counterpart) the JP CTaskWidget
// (JpTaskPage.h) loads its page through: the form registry (form name ->
// creator) and FrTaskForm, the form base of the JP task pages. Built on the
// KR Fresh (FrForm, frform.h); JP x86 offsets and VAs in comments, read from
// the JP exe (node scripts/disasm.ts --jp 0x<va>). The names are ours (the
// JP exe has no symbols); FrTaskForm's is its RTTI name.
#ifndef PG_JP_JPFRFORMS_H
#define PG_JP_JPFRFORMS_H

#include <map>
#include <string>

#include "defines.h"
#include "padding.h"
#include "frform.h"

class CTask;
class FrWnd;
class MsgObject;
class FrWndManager;

// The form registry: a function-local static (0xf1ac54, guard 0xf1ac64,
// destroyed at exit) that is just a std::map<std::string, creator>. The JP
// game fills it at static init, one `static bool` per form
// (`s_registered = JpFrFormRegistry::Instance()->Register(name, creator)`):
// 0xcd2220 "realmyroom" -> 0x620b90 (CRealMyRoomMain), 0xcd7dc0
// "tutorialmain" -> 0x7d5160 (CTutorialMain), 0xcea4a0 "shopmain" ->
// 0xa92d70 (CShopMain, RTTI 0xf02e2c : FrTaskForm, size 0x6c0: JpShopMain,
// JpShopMain.h). Each creator is `return CreateForm<T>(pManager, NULL,
// name, NULL);`.
class JpFrFormRegistry {
public:
	// __fastcall in the JP exe (name in ecx, manager in edx).
	typedef FrWnd* (*Creator)(const char* name, FrWndManager* pManager);

	static JpFrFormRegistry* Instance(void);                          // 0xc0eee0
	// Adds name -> creator; false (nothing added) when the name is taken.
	bool Register(const char* name, Creator creator);                 // 0xc0ebd0
	// The window name's creator builds, NULL for an unknown name.
	FrWnd* Create(const char* name, FrWndManager* pManager);          // 0xc0df10

private:
	std::map<std::string, Creator> m_creators; // 0x0
};
// size 0xc (x86)
CHECK_SIZE(JpFrFormRegistry, 0xc);

// The JP's second factory, the one FrChatDlg (0xcc8610 / 0x48f590) and
// FrRoomListSelector (0x59e4b0) register with, is no JP addition: it is the
// KR CObjectFactory (ObjectFactory.h, objectfactory.cpp), which the port
// already has and which the JP Fresh controls register with too
// (0xcf7c80: "FrStatic" -> 0xc02360, 786 registrations in all). Its JP
// build, read from the JP exe:
//   ObjectFactory()            0xa4c0d0 (function-local static 0xf00718,
//                                        guard 0xf00738, size 0x20: VS 2010
//                                        maps of 0x10)
//   CObjectFactory()           0xa4bf30 (inline body, the static's init)
//   ~CObjectFactory()          0xa4c030 (Reset 0xa4b890, then both maps)
//   AddObjectType              0xa4bc50 (m_objectTypes[name] = object)
//   AddObjectFunctor           0xa4bcf0 (m_objectFunctors[name] = make;
//                                        map operator[] 0xa4bab0)
//   CreateObject               0x4b85f0 (the copy the JP linker kept; KR
//                                        defines it in TaskManager.cpp)
// The JP registration is the KR one (frstatic.cpp's __sFrStatic): a static
// object whose constructor calls
// `ObjectFactory().AddObjectFunctor(<Class>MakeInstance, "<Class>")` with an
// `IObject* <Class>MakeInstance(void) { return new <Class>; }` creator.
// CreateObject's JP callers: the task manager's create-by-name 0x4ba2d0
// (31 calls from 0x4ba550), 0x9bf340 / 0x9bf550 / 0x9bf6e0 (names from the
// caller or from the run-time-filled name pointers 0xe76c1c ...), 0xa46ac0,
// and four singleton creators (0x4bc970, 0x55a3b0, 0x57d5e0, 0x5ef240) on
// the name pointers 0xe74c54, 0xe75148, 0xe75c00, 0xe78c0c.

// The JP task pages' form base (RTTI 0xe74f88 "FrTaskForm", static init
// 0xccdf20, base FrForm 0xf1aabc; vtable 0xd0877c). Three virtuals after the
// JP FrForm's 38 (JP slots 38-40); the KR FrForm has fewer slots, so the
// port's slot numbers differ from the JP comments. Size 0x120 (x86: 0x5238f0
// news 0x120 for a plain FrTaskForm, and the JP CShopMain 0xa91ec0 and the
// class at 0x61f1f0 have their second base at +0x120; the constructor sets
// nothing past +0x110).
class FrTaskForm : public FrForm {
	LAYOUT_CHECK_FRIEND;
public:
	static const WRTTI m_RTTI;                                        // 0xe74f88
	virtual const WRTTI* GetRTTI(void) const { return &m_RTTI; }

	FrTaskForm(void);                                                 // 0x523690
	// The page's start-up after its layout loaded; false fails the load
	// (CTaskWidget::Load sends the main actor game text 0x138a).
	virtual bool Start(void);                                         // [38] 0x5235f0
	// The page's tear-down (CTaskWidget::CloseForm).
	virtual bool Shutdown(void);                                      // [39] 0x523610
	// A task message (the KR CTaskMain::HandleMsg's argument) offered to the
	// page; true when handled. The JP CShopMain's 0xa971f0 is the KR
	// CShopMain::HandleMsg.
	virtual bool HandleMsg(const MsgObject& msg);                     // [40] 0x523620

protected:
	CTask* m_pTask; // 0x110 (the current task at construction)
	PADDING(0x114, 0x120);
};
CHECK_SIZE(FrTaskForm, 0x120);
BEGIN_LAYOUT_CHECK(FrTaskForm)
	CHECK_FIELD(FrTaskForm, m_pTask, 0x110);
END_LAYOUT_CHECK

// JpTaskPage.cpp glue (defined in JpFrForms.cpp): the registry's accessor
// and Create, and FrTaskForm's [38] Start / [39] Shutdown on a FrForm
// (Start fails, Shutdown does nothing, for a form that is no FrTaskForm).
JpFrFormRegistry* JpFrFormRegistry_Instance(void);
FrWnd* JpFrFormRegistry_Create(JpFrFormRegistry* registry, const char* name, FrWndManager* pManager);
bool JpFrTaskForm_Start(FrForm* form);
void JpFrTaskForm_Shutdown(FrForm* form);

#endif
