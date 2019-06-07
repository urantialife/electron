// Copyright (c) 2019 GitHub, Inc.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#include "atom/browser/api/atom_api_deprecate.h"

#include <string>

#include "atom/common/node_includes.h"
#include "base/bind_helpers.h"
#include "gin/dictionary.h"
#include "gin/function_template.h"

namespace {

v8::Maybe<bool> EmitWarning(node::Environment* env,
                            const char* warning,
                            const char* type = nullptr,
                            const char* code = nullptr) {
  v8::HandleScope handle_scope(env->isolate());
  v8::Context::Scope context_scope(env->context());

  v8::Local<v8::Object> process = env->process_object();
  v8::Local<v8::Value> emit_warning;
  if (!process->Get(env->context(), env->emit_warning_string())
           .ToLocal(&emit_warning)) {
    return v8::Nothing<bool>();
  }

  if (!emit_warning->IsFunction())
    return v8::Just(false);

  int argc = 0;
  v8::Local<v8::Value> args[3];  // warning, type, code

  if (!v8::String::NewFromUtf8(env->isolate(), warning,
                               v8::NewStringType::kNormal)
           .ToLocal(&args[argc++])) {
    return v8::Nothing<bool>();
  }
  if (type != nullptr) {
    if (!v8::String::NewFromOneByte(env->isolate(),
                                    reinterpret_cast<const uint8_t*>(type),
                                    v8::NewStringType::kNormal)
             .ToLocal(&args[argc++])) {
      return v8::Nothing<bool>();
    }
    if (code != nullptr &&
        !v8::String::NewFromOneByte(env->isolate(),
                                    reinterpret_cast<const uint8_t*>(code),
                                    v8::NewStringType::kNormal)
             .ToLocal(&args[argc++])) {
      return v8::Nothing<bool>();
    }
  }

  // MakeCallback() unneeded because emitWarning is internal code, it calls
  // process.emit('warning', ...), but does so on the nextTick.
  if (emit_warning.As<v8::Function>()
          ->Call(env->context(), process, argc, args)
          .IsEmpty()) {
    return v8::Nothing<bool>();
  }
  return v8::Just(true);
}

}  // namespace

namespace atom {

namespace api {

gin::WrapperInfo Deprecate::kWrapperInfo = {gin::kEmbedderNativeGin};

Deprecate::Deprecate(v8::Isolate* isolate) {}

Deprecate::~Deprecate() {}

// static
gin::Handle<Deprecate> Deprecate::Create(v8::Isolate* isolate) {
  return gin::CreateHandle(isolate, new Deprecate(isolate));
}

// static
void Deprecate::NativeWarn(v8::Isolate* isolate, const std::string& msg) {
  v8::Local<v8::Context> context = isolate->GetCurrentContext();
  node::Environment* env = node::Environment::GetCurrent(context);

  EmitWarning(env, msg.c_str(), "Electron");
}

gin::ObjectTemplateBuilder Deprecate::GetObjectTemplateBuilder(
    v8::Isolate* isolate) {
  return gin::Wrappable<Deprecate>::GetObjectTemplateBuilder(isolate).SetMethod(
      "warn", &Deprecate::NativeWarn);
}

}  // namespace api

}  // namespace atom

namespace {

void Initialize(v8::Local<v8::Object> exports,
                v8::Local<v8::Value> unused,
                v8::Local<v8::Context> context,
                void* priv) {
  v8::Isolate* isolate = context->GetIsolate();
  gin::Dictionary dict(isolate, exports);
  dict.Set("deprecateNative", atom::api::Deprecate::Create(isolate));
}

}  // namespace

NODE_LINKED_MODULE_CONTEXT_AWARE(atom_browser_deprecate, Initialize)
