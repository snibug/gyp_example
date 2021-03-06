/*
 * Copyright 2012 Google Inc. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <config.h>
#include <wtf/StackBounds.h>

#include <pthread.h>

namespace WTF {

void StackBounds::initialize() {
  void* stackBase = 0;
  size_t stackSize = 0;

  pthread_t thread = pthread_self();
  pthread_attr_t sattr;
  pthread_attr_init(&sattr);
  pthread_getattr_np(thread, &sattr);
  int rc = pthread_attr_getstack(&sattr, &stackBase, &stackSize);
  ASSERT(rc == 0);
  ASSERT(stackBase);
  pthread_attr_destroy(&sattr);
  m_bound = stackBase;
  m_origin = static_cast<char*>(stackBase) + stackSize;
}

}  // namespace WTF
