// Copyright (c) 2019 GitHub, Inc.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#ifndef ATOM_BROWSER_API_ATOM_API_DEPRECATE_H_
#define ATOM_BROWSER_API_ATOM_API_DEPRECATE_H_

#include <map>
#include <memory>

#include "gin/handle.h"
#include "gin/object_template_builder.h"
#include "gin/wrappable.h"

namespace atom {

namespace api {

class Deprecate : public gin::Wrappable<Deprecate> {
 public:
  static gin::Handle<Deprecate> Create(v8::Isolate* isolate);

  // gin::Wrappable
  gin::ObjectTemplateBuilder GetObjectTemplateBuilder(
      v8::Isolate* isolate) override;

  static gin::WrapperInfo kWrapperInfo;

  static void NativeWarn(v8::Isolate* isolate, const std::string& msg);

 protected:
  explicit Deprecate(v8::Isolate* isolate);
  ~Deprecate() override;

 private:
  DISALLOW_COPY_AND_ASSIGN(Deprecate);
};

}  // namespace api

}  // namespace atom

#endif  // ATOM_BROWSER_API_ATOM_API_DEPRECATE_H_
