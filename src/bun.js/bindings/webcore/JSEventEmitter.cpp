#include "config.h"
#include "JSEventEmitter.h"

#include "ActiveDOMObject.h"
#include "ExtendedDOMClientIsoSubspaces.h"
#include "ExtendedDOMIsoSubspaces.h"
#include "IDLTypes.h"
#include "JSAddEventListenerOptions.h"
#include "JSDOMBinding.h"
#include "JSDOMConstructor.h"
#include "JSDOMConvertBase.h"
#include "JSDOMConvertBoolean.h"
#include "JSDOMConvertDictionary.h"
#include "JSDOMConvertEventListener.h"
#include "JSDOMConvertInterface.h"
#include "JSDOMConvertNullable.h"
#include "JSDOMConvertStrings.h"
#include "JSDOMConvertUnion.h"
#include "JSDOMExceptionHandling.h"
#include "JSDOMGlobalObjectInlines.h"
#include "JSDOMOperation.h"
#include "JSDOMWrapperCache.h"
#include "JSEvent.h"
#include "JSEventListener.h"
#include "JSEventListenerOptions.h"
#include "ScriptExecutionContext.h"
#include "WebCoreJSClientData.h"
#include <JavaScriptCore/FunctionPrototype.h>
#include <JavaScriptCore/HeapAnalyzer.h>
#include <JavaScriptCore/Identifier.h>
#include <JavaScriptCore/JSCInlines.h>
#include <JavaScriptCore/JSDestructibleObjectHeapCellType.h>
#include <JavaScriptCore/SlotVisitorMacros.h>
#include <JavaScriptCore/SubspaceInlines.h>
#include <variant>
#include <wtf/GetPtr.h>
#include <wtf/PointerPreparations.h>
#include <wtf/URL.h>

namespace WebCore {
using namespace JSC;

// Functions

static JSC_DECLARE_HOST_FUNCTION(jsEventEmitterPrototypeFunction_addListener);
static JSC_DECLARE_HOST_FUNCTION(jsEventEmitterPrototypeFunction_addOnceListener);
static JSC_DECLARE_HOST_FUNCTION(jsEventEmitterPrototypeFunction_prependListener);
static JSC_DECLARE_HOST_FUNCTION(jsEventEmitterPrototypeFunction_prependOnceListener);
static JSC_DECLARE_HOST_FUNCTION(jsEventEmitterPrototypeFunction_removeListener);
static JSC_DECLARE_HOST_FUNCTION(jsEventEmitterPrototypeFunction_removeAllListeners);
static JSC_DECLARE_HOST_FUNCTION(jsEventEmitterPrototypeFunction_emit);
static JSC_DECLARE_HOST_FUNCTION(jsEventEmitterPrototypeFunction_eventNames);
static JSC_DECLARE_HOST_FUNCTION(jsEventEmitterPrototypeFunction_listenerCount);
static JSC_DECLARE_HOST_FUNCTION(jsEventEmitterPrototypeFunction_listeners);

// Attributes

static JSC_DECLARE_CUSTOM_GETTER(jsEventEmitterConstructor);

class JSEventEmitterPrototype final : public JSC::JSNonFinalObject {
public:
    using Base = JSC::JSNonFinalObject;
    static JSEventEmitterPrototype* create(JSC::VM& vm, JSDOMGlobalObject* globalObject, JSC::Structure* structure)
    {
        JSEventEmitterPrototype* ptr = new (NotNull, JSC::allocateCell<JSEventEmitterPrototype>(vm)) JSEventEmitterPrototype(vm, globalObject, structure);
        ptr->finishCreation(vm);
        return ptr;
    }

    DECLARE_INFO;
    template<typename CellType, JSC::SubspaceAccess>
    static JSC::GCClient::IsoSubspace* subspaceFor(JSC::VM& vm)
    {
        STATIC_ASSERT_ISO_SUBSPACE_SHARABLE(JSEventEmitterPrototype, Base);
        return &vm.plainObjectSpace();
    }
    static JSC::Structure* createStructure(JSC::VM& vm, JSC::JSGlobalObject* globalObject, JSC::JSValue prototype)
    {
        return JSC::Structure::create(vm, globalObject, prototype, JSC::TypeInfo(JSC::ObjectType, StructureFlags), info());
    }

private:
    JSEventEmitterPrototype(JSC::VM& vm, JSC::JSGlobalObject*, JSC::Structure* structure)
        : JSC::JSNonFinalObject(vm, structure)
    {
    }

    void finishCreation(JSC::VM&);

public:
    static constexpr unsigned StructureFlags = Base::StructureFlags | JSC::IsImmutablePrototypeExoticObject;
};
STATIC_ASSERT_ISO_SUBSPACE_SHARABLE(JSEventEmitterPrototype, JSEventEmitterPrototype::Base);

using JSEventEmitterDOMConstructor = JSDOMConstructor<JSEventEmitter>;

template<> EncodedJSValue JSC_HOST_CALL_ATTRIBUTES JSEventEmitterDOMConstructor::construct(JSGlobalObject* lexicalGlobalObject, CallFrame* callFrame)
{
    VM& vm = lexicalGlobalObject->vm();
    auto throwScope = DECLARE_THROW_SCOPE(vm);
    auto* castedThis = jsCast<JSEventEmitterDOMConstructor*>(callFrame->jsCallee());
    ASSERT(castedThis);
    auto* context = castedThis->scriptExecutionContext();
    if (UNLIKELY(!context))
        return throwConstructorScriptExecutionContextUnavailableError(*lexicalGlobalObject, throwScope, "EventEmitter");
    auto object = EventEmitter::create(*context);
    if constexpr (IsExceptionOr<decltype(object)>)
        RETURN_IF_EXCEPTION(throwScope, {});
    static_assert(TypeOrExceptionOrUnderlyingType<decltype(object)>::isRef);
    auto jsValue = toJSNewlyCreated<IDLInterface<EventEmitter>>(*lexicalGlobalObject, *castedThis->globalObject(), throwScope, WTFMove(object));
    if constexpr (IsExceptionOr<decltype(object)>)
        RETURN_IF_EXCEPTION(throwScope, {});
    setSubclassStructureIfNeeded<EventEmitter>(lexicalGlobalObject, callFrame, asObject(jsValue));
    RETURN_IF_EXCEPTION(throwScope, {});
    return JSValue::encode(jsValue);
}
JSC_ANNOTATE_HOST_FUNCTION(JSEventEmitterDOMConstructorConstruct, JSEventEmitterDOMConstructor::construct);

template<> const ClassInfo JSEventEmitterDOMConstructor::s_info = { "EventEmitter"_s, &Base::s_info, nullptr, nullptr, CREATE_METHOD_TABLE(JSEventEmitterDOMConstructor) };

template<> JSValue JSEventEmitterDOMConstructor::prototypeForStructure(JSC::VM& vm, const JSDOMGlobalObject& globalObject)
{
    UNUSED_PARAM(vm);
    return globalObject.functionPrototype();
}

template<> void JSEventEmitterDOMConstructor::initializeProperties(VM& vm, JSDOMGlobalObject& globalObject)
{
    putDirect(vm, vm.propertyNames->length, jsNumber(0), JSC::PropertyAttribute::ReadOnly | JSC::PropertyAttribute::DontEnum);
    JSString* nameString = jsNontrivialString(vm, "EventEmitter"_s);
    m_originalName.set(vm, this, nameString);
    putDirect(vm, vm.propertyNames->name, nameString, JSC::PropertyAttribute::ReadOnly | JSC::PropertyAttribute::DontEnum);
    putDirect(vm, vm.propertyNames->prototype, JSEventEmitter::prototype(vm, globalObject), JSC::PropertyAttribute::ReadOnly | JSC::PropertyAttribute::DontEnum | JSC::PropertyAttribute::DontDelete);
}

/* Hash table for prototype */

static const HashTableValue JSEventEmitterPrototypeTableValues[] = {
    { "constructor"_s, static_cast<unsigned>(JSC::PropertyAttribute::DontEnum), NoIntrinsic, { (intptr_t) static_cast<PropertySlot::GetValueFunc>(jsEventEmitterConstructor), (intptr_t) static_cast<PutPropertySlot::PutValueFunc>(0) } },
    { "addListener"_s, static_cast<unsigned>(JSC::PropertyAttribute::Function), NoIntrinsic, { (intptr_t) static_cast<RawNativeFunction>(jsEventEmitterPrototypeFunction_addListener), (intptr_t)(2) } },
    { "on"_s, static_cast<unsigned>(JSC::PropertyAttribute::Function), NoIntrinsic, { (intptr_t) static_cast<RawNativeFunction>(jsEventEmitterPrototypeFunction_addListener), (intptr_t)(2) } },
    { "once"_s, static_cast<unsigned>(JSC::PropertyAttribute::Function), NoIntrinsic, { (intptr_t) static_cast<RawNativeFunction>(jsEventEmitterPrototypeFunction_addOnceListener), (intptr_t)(2) } },
    { "prepend"_s, static_cast<unsigned>(JSC::PropertyAttribute::Function), NoIntrinsic, { (intptr_t) static_cast<RawNativeFunction>(jsEventEmitterPrototypeFunction_prependListener), (intptr_t)(2) } },
    { "prependOnce"_s, static_cast<unsigned>(JSC::PropertyAttribute::Function), NoIntrinsic, { (intptr_t) static_cast<RawNativeFunction>(jsEventEmitterPrototypeFunction_prependOnceListener), (intptr_t)(2) } },
    { "removeListener"_s, static_cast<unsigned>(JSC::PropertyAttribute::Function), NoIntrinsic, { (intptr_t) static_cast<RawNativeFunction>(jsEventEmitterPrototypeFunction_removeListener), (intptr_t)(2) } },
    { "off"_s, static_cast<unsigned>(JSC::PropertyAttribute::Function), NoIntrinsic, { (intptr_t) static_cast<RawNativeFunction>(jsEventEmitterPrototypeFunction_removeListener), (intptr_t)(2) } },
    { "removeAllListeners"_s, static_cast<unsigned>(JSC::PropertyAttribute::Function), NoIntrinsic, { (intptr_t) static_cast<RawNativeFunction>(jsEventEmitterPrototypeFunction_removeAllListeners), (intptr_t)(1) } },
    { "emit"_s, static_cast<unsigned>(JSC::PropertyAttribute::Function), NoIntrinsic, { (intptr_t) static_cast<RawNativeFunction>(jsEventEmitterPrototypeFunction_emit), (intptr_t)(1) } },
    { "eventNames"_s, static_cast<unsigned>(JSC::PropertyAttribute::Function), NoIntrinsic, { (intptr_t) static_cast<RawNativeFunction>(jsEventEmitterPrototypeFunction_eventNames), (intptr_t)(0) } },
    { "listenerCount"_s, static_cast<unsigned>(JSC::PropertyAttribute::Function), NoIntrinsic, { (intptr_t) static_cast<RawNativeFunction>(jsEventEmitterPrototypeFunction_listenerCount), (intptr_t)(1) } },
    { "listeners"_s, static_cast<unsigned>(JSC::PropertyAttribute::Function), NoIntrinsic, { (intptr_t) static_cast<RawNativeFunction>(jsEventEmitterPrototypeFunction_listeners), (intptr_t)(1) } },
    // Need to double check the difference between rawListeners and listeners.
    { "rawListeners"_s, static_cast<unsigned>(JSC::PropertyAttribute::Function), NoIntrinsic, { (intptr_t) static_cast<RawNativeFunction>(jsEventEmitterPrototypeFunction_listeners), (intptr_t)(1) } },
};

const ClassInfo JSEventEmitterPrototype::s_info = { "EventEmitter"_s, &Base::s_info, nullptr, nullptr, CREATE_METHOD_TABLE(JSEventEmitterPrototype) };

void JSEventEmitterPrototype::finishCreation(VM& vm)
{
    Base::finishCreation(vm);
    reifyStaticProperties(vm, JSEventEmitter::info(), JSEventEmitterPrototypeTableValues, *this);
    JSC_TO_STRING_TAG_WITHOUT_TRANSITION();
}

const ClassInfo JSEventEmitter::s_info = { "EventEmitter"_s, &Base::s_info, nullptr, nullptr, CREATE_METHOD_TABLE(JSEventEmitter) };

JSEventEmitter::JSEventEmitter(Structure* structure, JSDOMGlobalObject& globalObject, Ref<EventEmitter>&& impl)
    : JSDOMWrapper<EventEmitter>(structure, globalObject, WTFMove(impl))
{
}

void JSEventEmitter::finishCreation(VM& vm)
{
    Base::finishCreation(vm);
    ASSERT(inherits(info()));

    // static_assert(!std::is_base_of<ActiveDOMObject, EventEmitter>::value, "Interface is not marked as [ActiveDOMObject] even though implementation class subclasses ActiveDOMObject.");
}

JSObject* JSEventEmitter::createPrototype(VM& vm, JSDOMGlobalObject& globalObject)
{
    return JSEventEmitterPrototype::create(vm, &globalObject, JSEventEmitterPrototype::createStructure(vm, &globalObject, globalObject.objectPrototype()));
}

JSObject* JSEventEmitter::prototype(VM& vm, JSDOMGlobalObject& globalObject)
{
    return getDOMPrototype<JSEventEmitter>(vm, globalObject);
}

JSValue JSEventEmitter::getConstructor(VM& vm, const JSGlobalObject* globalObject)
{
    return getDOMConstructor<JSEventEmitterDOMConstructor, DOMConstructorID::EventEmitter>(vm, *jsCast<const JSDOMGlobalObject*>(globalObject));
}

void JSEventEmitter::destroy(JSC::JSCell* cell)
{
    JSEventEmitter* thisObject = static_cast<JSEventEmitter*>(cell);
    thisObject->JSEventEmitter::~JSEventEmitter();
}

JSC_DEFINE_CUSTOM_GETTER(jsEventEmitterConstructor, (JSGlobalObject * lexicalGlobalObject, EncodedJSValue thisValue, PropertyName))
{
    VM& vm = JSC::getVM(lexicalGlobalObject);
    auto throwScope = DECLARE_THROW_SCOPE(vm);
    auto* prototype = jsDynamicCast<JSEventEmitterPrototype*>(JSValue::decode(thisValue));
    if (UNLIKELY(!prototype))
        return throwVMTypeError(lexicalGlobalObject, throwScope);
    return JSValue::encode(JSEventEmitter::getConstructor(JSC::getVM(lexicalGlobalObject), prototype->globalObject()));
}

static inline JSC::EncodedJSValue addListener(JSC::JSGlobalObject* lexicalGlobalObject, JSC::CallFrame* callFrame, typename IDLOperation<JSEventEmitter>::ClassParameter castedThis, bool once, bool prepend)
{
    auto& vm = JSC::getVM(lexicalGlobalObject);
    auto throwScope = DECLARE_THROW_SCOPE(vm);
    UNUSED_PARAM(throwScope);
    UNUSED_PARAM(callFrame);
    auto& impl = castedThis->wrapped();
    if (UNLIKELY(callFrame->argumentCount() < 2))
        return throwVMError(lexicalGlobalObject, throwScope, createNotEnoughArgumentsError(lexicalGlobalObject));
    EnsureStillAliveScope argument0 = callFrame->uncheckedArgument(0);
    auto eventType = argument0.value().toPropertyKey(lexicalGlobalObject);
    RETURN_IF_EXCEPTION(throwScope, encodedJSValue());
    EnsureStillAliveScope argument1 = callFrame->uncheckedArgument(1);
    auto listener = convert<IDLNullable<IDLEventListener<JSEventListener>>>(*lexicalGlobalObject, argument1.value(), *castedThis, [](JSC::JSGlobalObject& lexicalGlobalObject, JSC::ThrowScope& scope) { throwArgumentMustBeObjectError(lexicalGlobalObject, scope, 1, "listener", "EventEmitter", "addListener"); });
    RETURN_IF_EXCEPTION(throwScope, encodedJSValue());
    auto result = JSValue::encode(toJS<IDLUndefined>(*lexicalGlobalObject, throwScope, [&]() -> decltype(auto) { return impl.addListenerForBindings(WTFMove(eventType), WTFMove(listener), once, prepend); }));
    RETURN_IF_EXCEPTION(throwScope, encodedJSValue());
    vm.writeBarrier(&static_cast<JSObject&>(*castedThis), argument1.value());
    return result;
}

static inline JSC::EncodedJSValue jsEventEmitterPrototypeFunction_addListenerBody(JSC::JSGlobalObject* lexicalGlobalObject, JSC::CallFrame* callFrame, typename IDLOperation<JSEventEmitter>::ClassParameter castedThis)
{
    return addListener(lexicalGlobalObject, callFrame, castedThis, false, false);
}

static inline JSC::EncodedJSValue jsEventEmitterPrototypeFunction_addOnceListenerBody(JSC::JSGlobalObject* lexicalGlobalObject, JSC::CallFrame* callFrame, typename IDLOperation<JSEventEmitter>::ClassParameter castedThis)
{
    return addListener(lexicalGlobalObject, callFrame, castedThis, true, false);
}

static inline JSC::EncodedJSValue jsEventEmitterPrototypeFunction_prependListenerBody(JSC::JSGlobalObject* lexicalGlobalObject, JSC::CallFrame* callFrame, typename IDLOperation<JSEventEmitter>::ClassParameter castedThis)
{
    return addListener(lexicalGlobalObject, callFrame, castedThis, false, true);
}

static inline JSC::EncodedJSValue jsEventEmitterPrototypeFunction_prependOnceListenerBody(JSC::JSGlobalObject* lexicalGlobalObject, JSC::CallFrame* callFrame, typename IDLOperation<JSEventEmitter>::ClassParameter castedThis)
{
    return addListener(lexicalGlobalObject, callFrame, castedThis, true, true);
}

JSC_DEFINE_HOST_FUNCTION(jsEventEmitterPrototypeFunction_addListener, (JSGlobalObject * lexicalGlobalObject, CallFrame* callFrame))
{
    return IDLOperation<JSEventEmitter>::call<jsEventEmitterPrototypeFunction_addListenerBody>(*lexicalGlobalObject, *callFrame, "addListener");
}

JSC_DEFINE_HOST_FUNCTION(jsEventEmitterPrototypeFunction_addOnceListener, (JSGlobalObject * lexicalGlobalObject, CallFrame* callFrame))
{
    return IDLOperation<JSEventEmitter>::call<jsEventEmitterPrototypeFunction_addOnceListenerBody>(*lexicalGlobalObject, *callFrame, "once");
}

JSC_DEFINE_HOST_FUNCTION(jsEventEmitterPrototypeFunction_prependListener, (JSGlobalObject * lexicalGlobalObject, CallFrame* callFrame))
{
    return IDLOperation<JSEventEmitter>::call<jsEventEmitterPrototypeFunction_prependListenerBody>(*lexicalGlobalObject, *callFrame, "prependListener");
}

JSC_DEFINE_HOST_FUNCTION(jsEventEmitterPrototypeFunction_prependOnceListener, (JSGlobalObject * lexicalGlobalObject, CallFrame* callFrame))
{
    return IDLOperation<JSEventEmitter>::call<jsEventEmitterPrototypeFunction_prependOnceListenerBody>(*lexicalGlobalObject, *callFrame, "prependOnceListener");
}

static inline JSC::EncodedJSValue jsEventEmitterPrototypeFunction_removeListenerBody(JSC::JSGlobalObject* lexicalGlobalObject, JSC::CallFrame* callFrame, typename IDLOperation<JSEventEmitter>::ClassParameter castedThis)
{
    auto& vm = JSC::getVM(lexicalGlobalObject);
    auto throwScope = DECLARE_THROW_SCOPE(vm);
    UNUSED_PARAM(throwScope);
    UNUSED_PARAM(callFrame);
    auto& impl = castedThis->wrapped();
    if (UNLIKELY(callFrame->argumentCount() < 2))
        return throwVMError(lexicalGlobalObject, throwScope, createNotEnoughArgumentsError(lexicalGlobalObject));
    EnsureStillAliveScope argument0 = callFrame->uncheckedArgument(0);
    auto eventType = argument0.value().toPropertyKey(lexicalGlobalObject);
    RETURN_IF_EXCEPTION(throwScope, encodedJSValue());
    EnsureStillAliveScope argument1 = callFrame->uncheckedArgument(1);
    auto listener = convert<IDLNullable<IDLEventListener<JSEventListener>>>(*lexicalGlobalObject, argument1.value(), *castedThis, [](JSC::JSGlobalObject& lexicalGlobalObject, JSC::ThrowScope& scope) { throwArgumentMustBeObjectError(lexicalGlobalObject, scope, 1, "listener", "EventEmitter", "removeListener"); });
    RETURN_IF_EXCEPTION(throwScope, encodedJSValue());
    auto result = JSValue::encode(toJS<IDLUndefined>(*lexicalGlobalObject, throwScope, [&]() -> decltype(auto) { return impl.removeListenerForBindings(WTFMove(eventType), WTFMove(listener)); }));
    RETURN_IF_EXCEPTION(throwScope, encodedJSValue());
    vm.writeBarrier(&static_cast<JSObject&>(*castedThis), argument1.value());
    return result;
}

JSC_DEFINE_HOST_FUNCTION(jsEventEmitterPrototypeFunction_removeListener, (JSGlobalObject * lexicalGlobalObject, CallFrame* callFrame))
{
    return IDLOperation<JSEventEmitter>::call<jsEventEmitterPrototypeFunction_removeListenerBody>(*lexicalGlobalObject, *callFrame, "removeListener");
}

static inline JSC::EncodedJSValue jsEventEmitterPrototypeFunction_removeAllListenersBody(JSC::JSGlobalObject* lexicalGlobalObject, JSC::CallFrame* callFrame, typename IDLOperation<JSEventEmitter>::ClassParameter castedThis)
{
    auto& vm = JSC::getVM(lexicalGlobalObject);
    auto throwScope = DECLARE_THROW_SCOPE(vm);
    UNUSED_PARAM(throwScope);
    UNUSED_PARAM(callFrame);
    auto& impl = castedThis->wrapped();
    if (UNLIKELY(callFrame->argumentCount() < 1))
        return throwVMError(lexicalGlobalObject, throwScope, createNotEnoughArgumentsError(lexicalGlobalObject));
    EnsureStillAliveScope argument0 = callFrame->uncheckedArgument(0);
    auto eventType = argument0.value().toPropertyKey(lexicalGlobalObject);
    RETURN_IF_EXCEPTION(throwScope, encodedJSValue());
    auto result = JSValue::encode(toJS<IDLUndefined>(*lexicalGlobalObject, throwScope, [&]() -> decltype(auto) { return impl.removeAllListenersForBindings(WTFMove(eventType)); }));
    RETURN_IF_EXCEPTION(throwScope, encodedJSValue());
    return result;
}

JSC_DEFINE_HOST_FUNCTION(jsEventEmitterPrototypeFunction_removeAllListeners, (JSGlobalObject * lexicalGlobalObject, CallFrame* callFrame))
{
    return IDLOperation<JSEventEmitter>::call<jsEventEmitterPrototypeFunction_removeAllListenersBody>(*lexicalGlobalObject, *callFrame, "removeAllListeners");
}

static inline JSC::EncodedJSValue jsEventEmitterPrototypeFunction_emitBody(JSC::JSGlobalObject* lexicalGlobalObject, JSC::CallFrame* callFrame, typename IDLOperation<JSEventEmitter>::ClassParameter castedThis)
{
    auto& vm = JSC::getVM(lexicalGlobalObject);
    auto throwScope = DECLARE_THROW_SCOPE(vm);
    UNUSED_PARAM(throwScope);
    UNUSED_PARAM(callFrame);
    auto& impl = castedThis->wrapped();
    size_t argumentCount = callFrame->argumentCount();
    if (UNLIKELY(argumentCount < 1))
        return throwVMError(lexicalGlobalObject, throwScope, createNotEnoughArgumentsError(lexicalGlobalObject));
    auto eventType = callFrame->uncheckedArgument(0).toPropertyKey(lexicalGlobalObject);
    RETURN_IF_EXCEPTION(throwScope, encodedJSValue());
    JSC::MarkedArgumentBuffer args;
    for (size_t i = 1; i < argumentCount; ++i) {
        args.append(callFrame->uncheckedArgument(i));
    }
    RELEASE_AND_RETURN(throwScope, JSValue::encode(toJS<IDLBoolean>(*lexicalGlobalObject, throwScope, impl.emitForBindings(eventType, args))));
}

JSC_DEFINE_HOST_FUNCTION(jsEventEmitterPrototypeFunction_emit, (JSGlobalObject * lexicalGlobalObject, CallFrame* callFrame))
{
    return IDLOperation<JSEventEmitter>::call<jsEventEmitterPrototypeFunction_emitBody>(*lexicalGlobalObject, *callFrame, "emit");
}

static inline JSC::EncodedJSValue jsEventEmitterPrototypeFunction_eventNamesBody(JSC::JSGlobalObject* lexicalGlobalObject, JSC::CallFrame* callFrame, typename IDLOperation<JSEventEmitter>::ClassParameter castedThis)
{
    auto& vm = JSC::getVM(lexicalGlobalObject);
    auto throwScope = DECLARE_THROW_SCOPE(vm);
    UNUSED_PARAM(throwScope);
    UNUSED_PARAM(callFrame);
    auto& impl = castedThis->wrapped();
    JSC::MarkedArgumentBuffer args;
    for (auto& name : impl.getEventNames()) {
        args.append(JSC::identifierToSafePublicJSValue(vm, name));
    }
    RELEASE_AND_RETURN(throwScope, JSC::JSValue::encode(JSC::constructArray(lexicalGlobalObject, static_cast<JSC::ArrayAllocationProfile*>(nullptr), WTFMove(args))));
}

JSC_DEFINE_HOST_FUNCTION(jsEventEmitterPrototypeFunction_eventNames, (JSGlobalObject * lexicalGlobalObject, CallFrame* callFrame))
{
    return IDLOperation<JSEventEmitter>::call<jsEventEmitterPrototypeFunction_eventNamesBody>(*lexicalGlobalObject, *callFrame, "eventNames");
}

static inline JSC::EncodedJSValue jsEventEmitterPrototypeFunction_listenerCountBody(JSC::JSGlobalObject* lexicalGlobalObject, JSC::CallFrame* callFrame, typename IDLOperation<JSEventEmitter>::ClassParameter castedThis)
{
    auto& vm = JSC::getVM(lexicalGlobalObject);
    auto throwScope = DECLARE_THROW_SCOPE(vm);
    UNUSED_PARAM(throwScope);
    UNUSED_PARAM(callFrame);
    auto& impl = castedThis->wrapped();
    if (UNLIKELY(callFrame->argumentCount() < 1))
        return throwVMError(lexicalGlobalObject, throwScope, createNotEnoughArgumentsError(lexicalGlobalObject));
    auto eventType = callFrame->uncheckedArgument(0).toPropertyKey(lexicalGlobalObject);
    RETURN_IF_EXCEPTION(throwScope, encodedJSValue());
    RELEASE_AND_RETURN(throwScope, JSC::JSValue::encode(JSC::jsNumber(impl.listenerCount(eventType))));
}

JSC_DEFINE_HOST_FUNCTION(jsEventEmitterPrototypeFunction_listenerCount, (JSGlobalObject * lexicalGlobalObject, CallFrame* callFrame))
{
    return IDLOperation<JSEventEmitter>::call<jsEventEmitterPrototypeFunction_listenerCountBody>(*lexicalGlobalObject, *callFrame, "listeners");
}

static inline JSC::EncodedJSValue jsEventEmitterPrototypeFunction_listenersBody(JSC::JSGlobalObject* lexicalGlobalObject, JSC::CallFrame* callFrame, typename IDLOperation<JSEventEmitter>::ClassParameter castedThis)
{
    auto& vm = JSC::getVM(lexicalGlobalObject);
    auto throwScope = DECLARE_THROW_SCOPE(vm);
    UNUSED_PARAM(throwScope);
    UNUSED_PARAM(callFrame);
    auto& impl = castedThis->wrapped();
    if (UNLIKELY(callFrame->argumentCount() < 1))
        return throwVMError(lexicalGlobalObject, throwScope, createNotEnoughArgumentsError(lexicalGlobalObject));
    auto eventType = callFrame->uncheckedArgument(0).toPropertyKey(lexicalGlobalObject);
    RETURN_IF_EXCEPTION(throwScope, encodedJSValue());
    JSC::MarkedArgumentBuffer args;
    for (auto* listener : impl.getListeners(eventType)) {
        args.append(listener);
    }
    RELEASE_AND_RETURN(throwScope, JSC::JSValue::encode(JSC::constructArray(lexicalGlobalObject, static_cast<JSC::ArrayAllocationProfile*>(nullptr), WTFMove(args))));
}

JSC_DEFINE_HOST_FUNCTION(jsEventEmitterPrototypeFunction_listeners, (JSGlobalObject * lexicalGlobalObject, CallFrame* callFrame))
{
    return IDLOperation<JSEventEmitter>::call<jsEventEmitterPrototypeFunction_listenersBody>(*lexicalGlobalObject, *callFrame, "listeners");
}

JSC::GCClient::IsoSubspace* JSEventEmitter::subspaceForImpl(JSC::VM& vm)
{
    return WebCore::subspaceForImpl<JSEventEmitter, UseCustomHeapCellType::No>(
        vm,
        [](auto& spaces) { return spaces.m_clientSubspaceForEventEmitter.get(); },
        [](auto& spaces, auto&& space) { spaces.m_clientSubspaceForEventEmitter = WTFMove(space); },
        [](auto& spaces) { return spaces.m_subspaceForEventEmitter.get(); },
        [](auto& spaces, auto&& space) { spaces.m_subspaceForEventEmitter = WTFMove(space); });
}

template<typename Visitor>
void JSEventEmitter::visitChildrenImpl(JSCell* cell, Visitor& visitor)
{
    auto* thisObject = jsCast<JSEventEmitter*>(cell);
    ASSERT_GC_OBJECT_INHERITS(thisObject, info());
    Base::visitChildren(thisObject, visitor);
    thisObject->visitAdditionalChildren(visitor);
}

DEFINE_VISIT_CHILDREN(JSEventEmitter);

template<typename Visitor>
void JSEventEmitter::visitOutputConstraints(JSCell* cell, Visitor& visitor)
{
    auto* thisObject = jsCast<JSEventEmitter*>(cell);
    ASSERT_GC_OBJECT_INHERITS(thisObject, info());
    Base::visitOutputConstraints(thisObject, visitor);
    thisObject->visitAdditionalChildren(visitor);
}

template void JSEventEmitter::visitOutputConstraints(JSCell*, AbstractSlotVisitor&);
template void JSEventEmitter::visitOutputConstraints(JSCell*, SlotVisitor&);
void JSEventEmitter::analyzeHeap(JSCell* cell, HeapAnalyzer& analyzer)
{
    auto* thisObject = jsCast<JSEventEmitter*>(cell);
    analyzer.setWrappedObjectForCell(cell, &thisObject->wrapped());
    if (thisObject->scriptExecutionContext())
        analyzer.setLabelForCell(cell, "url " + thisObject->scriptExecutionContext()->url().string());
    Base::analyzeHeap(cell, analyzer);
}

bool JSEventEmitterOwner::isReachableFromOpaqueRoots(JSC::Handle<JSC::Unknown> handle, void*, AbstractSlotVisitor& visitor, const char** reason)
{
    auto* jsEventEmitter = jsCast<JSEventEmitter*>(handle.slot()->asCell());
    if (jsEventEmitter->wrapped().isFiringEventListeners()) {
        if (UNLIKELY(reason))
            *reason = "EventEmitter firing event listeners";
        return true;
    }
    UNUSED_PARAM(visitor);
    UNUSED_PARAM(reason);
    return false;
}

void JSEventEmitterOwner::finalize(JSC::Handle<JSC::Unknown> handle, void* context)
{
    auto* jsEventEmitter = static_cast<JSEventEmitter*>(handle.slot()->asCell());
    auto& world = *static_cast<DOMWrapperWorld*>(context);
    uncacheWrapper(world, &jsEventEmitter->wrapped(), jsEventEmitter);
}

JSC_DEFINE_HOST_FUNCTION(Events_functionGetEventListeners,
    (JSC::JSGlobalObject * lexicalGlobalObject, JSC::CallFrame* callFrame))
{
    auto& vm = JSC::getVM(lexicalGlobalObject);
    auto throwScope = DECLARE_THROW_SCOPE(vm);
    UNUSED_PARAM(throwScope);
    UNUSED_PARAM(callFrame);
    if (UNLIKELY(callFrame->argumentCount() < 2))
        return throwVMError(lexicalGlobalObject, throwScope, createNotEnoughArgumentsError(lexicalGlobalObject));
    auto* argument0 = JSC::jsDynamicCast<JSEventEmitter*>(callFrame->uncheckedArgument(0));
    if (UNLIKELY(!argument0)) {
        throwException(lexicalGlobalObject, throwScope, createError(lexicalGlobalObject, "Expected EventEmitter"_s));
        return JSValue::encode(JSC::jsUndefined());
    }
    auto* impl = JSEventEmitter::toWrapped(vm, argument0);
    auto eventType = callFrame->uncheckedArgument(1).toPropertyKey(lexicalGlobalObject);
    RETURN_IF_EXCEPTION(throwScope, encodedJSValue());
    JSC::MarkedArgumentBuffer args;
    for (auto* listener : impl->getListeners(eventType)) {
        args.append(listener);
    }
    RELEASE_AND_RETURN(throwScope, JSC::JSValue::encode(JSC::constructArray(lexicalGlobalObject, static_cast<JSC::ArrayAllocationProfile*>(nullptr), WTFMove(args))));
}

JSC_DEFINE_HOST_FUNCTION(Events_functionListenerCount,
    (JSC::JSGlobalObject * lexicalGlobalObject, JSC::CallFrame* callFrame))
{
    auto& vm = JSC::getVM(lexicalGlobalObject);
    auto throwScope = DECLARE_THROW_SCOPE(vm);
    UNUSED_PARAM(throwScope);
    UNUSED_PARAM(callFrame);
    if (UNLIKELY(callFrame->argumentCount() < 2))
        return throwVMError(lexicalGlobalObject, throwScope, createNotEnoughArgumentsError(lexicalGlobalObject));
    auto* argument0 = JSC::jsDynamicCast<JSEventEmitter*>(callFrame->uncheckedArgument(0));
    if (UNLIKELY(!argument0)) {
        throwException(lexicalGlobalObject, throwScope, createError(lexicalGlobalObject, "Expected EventEmitter"_s));
        return JSValue::encode(JSC::jsUndefined());
    }
    auto* impl = JSEventEmitter::toWrapped(vm, argument0);
    auto eventType = callFrame->uncheckedArgument(1).toPropertyKey(lexicalGlobalObject);
    RETURN_IF_EXCEPTION(throwScope, encodedJSValue());
    RELEASE_AND_RETURN(throwScope, JSC::JSValue::encode(JSC::jsNumber(impl->listenerCount(eventType))));
}

JSC_DEFINE_HOST_FUNCTION(Events_functionOnce,
    (JSC::JSGlobalObject * lexicalGlobalObject, JSC::CallFrame* callFrame))
{
    auto& vm = JSC::getVM(lexicalGlobalObject);
    auto throwScope = DECLARE_THROW_SCOPE(vm);
    UNUSED_PARAM(throwScope);
    UNUSED_PARAM(callFrame);

    if (UNLIKELY(callFrame->argumentCount() < 3))
        return throwVMError(lexicalGlobalObject, throwScope, createNotEnoughArgumentsError(lexicalGlobalObject));
    auto* argument0 = JSC::jsDynamicCast<JSEventEmitter*>(callFrame->uncheckedArgument(0));
    if (UNLIKELY(!argument0)) {
        throwException(lexicalGlobalObject, throwScope, createError(lexicalGlobalObject, "Expected EventEmitter"_s));
        return JSValue::encode(JSC::jsUndefined());
    }
    auto* impl = JSEventEmitter::toWrapped(vm, argument0);
    auto eventType = callFrame->uncheckedArgument(1).toPropertyKey(lexicalGlobalObject);
    RETURN_IF_EXCEPTION(throwScope, encodedJSValue());
    EnsureStillAliveScope argument2 = callFrame->uncheckedArgument(2);
    auto listener = convert<IDLNullable<IDLEventListener<JSEventListener>>>(*lexicalGlobalObject, argument2.value(), *argument0, [](JSC::JSGlobalObject& lexicalGlobalObject, JSC::ThrowScope& scope) { throwArgumentMustBeObjectError(lexicalGlobalObject, scope, 2, "listener", "EventEmitter", "removeListener"); });
    RETURN_IF_EXCEPTION(throwScope, encodedJSValue());
    auto result = JSValue::encode(toJS<IDLUndefined>(*lexicalGlobalObject, throwScope, [&]() -> decltype(auto) { return impl->addListenerForBindings(WTFMove(eventType), WTFMove(listener), true, false); }));
    RETURN_IF_EXCEPTION(throwScope, encodedJSValue());
    vm.writeBarrier(argument0, argument2.value());
    return result;
}

JSC_DEFINE_HOST_FUNCTION(Events_functionOn,
    (JSC::JSGlobalObject * lexicalGlobalObject, JSC::CallFrame* callFrame))
{
    auto& vm = JSC::getVM(lexicalGlobalObject);
    auto throwScope = DECLARE_THROW_SCOPE(vm);
    UNUSED_PARAM(throwScope);
    UNUSED_PARAM(callFrame);

    if (UNLIKELY(callFrame->argumentCount() < 3))
        return throwVMError(lexicalGlobalObject, throwScope, createNotEnoughArgumentsError(lexicalGlobalObject));
    auto* argument0 = JSC::jsDynamicCast<JSEventEmitter*>(callFrame->uncheckedArgument(0));
    if (UNLIKELY(!argument0)) {
        throwException(lexicalGlobalObject, throwScope, createError(lexicalGlobalObject, "Expected EventEmitter"_s));
        return JSValue::encode(JSC::jsUndefined());
    }
    auto* impl = JSEventEmitter::toWrapped(vm, argument0);
    auto eventType = callFrame->uncheckedArgument(1).toPropertyKey(lexicalGlobalObject);
    RETURN_IF_EXCEPTION(throwScope, encodedJSValue());
    EnsureStillAliveScope argument2 = callFrame->uncheckedArgument(2);
    auto listener = convert<IDLNullable<IDLEventListener<JSEventListener>>>(*lexicalGlobalObject, argument2.value(), *argument0, [](JSC::JSGlobalObject& lexicalGlobalObject, JSC::ThrowScope& scope) { throwArgumentMustBeObjectError(lexicalGlobalObject, scope, 2, "listener", "EventEmitter", "removeListener"); });
    RETURN_IF_EXCEPTION(throwScope, encodedJSValue());
    auto result = JSValue::encode(toJS<IDLUndefined>(*lexicalGlobalObject, throwScope, [&]() -> decltype(auto) { return impl->addListenerForBindings(WTFMove(eventType), WTFMove(listener), false, false); }));
    RETURN_IF_EXCEPTION(throwScope, encodedJSValue());
    vm.writeBarrier(argument0, argument2.value());
    return result;
}

}