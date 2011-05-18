/*
 * Copyright 2010, The Android Open Source Project
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

#ifndef BCC_SCRIPT_H
#define BCC_SCRIPT_H

#include <bcc/bcc.h>
#include "bcc_internal.h"

#include "Compiler.h"

#include <stddef.h>

namespace llvm {
  class Module;
}

namespace bcc {
  class ScriptCompiled;
  class ScriptCached;
  class SourceInfo;

  namespace ScriptStatus {
    enum StatusType {
      Unknown,
      Compiled,
#if USE_CACHE
      Cached,
#endif
    };
  }

  class Script {
  private:
    int mErrorCode;

    ScriptStatus::StatusType mStatus;

    union {
      ScriptCompiled *mCompiled;
#if USE_CACHE
      ScriptCached *mCached;
#endif
    };

    char const *mCachePath;

    char const *mResName;

    bool mIsContextSlotNotAvail;

    // Source List
    SourceInfo *mSourceList[2];
    // Note: mSourceList[0] (main source)
    // Note: mSourceList[1] (library source)
    // TODO(logan): Generalize this, use vector or SmallVector instead!

    // Register Symbol Lookup Function
    BCCSymbolLookupFn mpExtSymbolLookupFn;
    void *mpExtSymbolLookupFnContext;

  public:
    Script() : mErrorCode(BCC_NO_ERROR), mStatus(ScriptStatus::Unknown),
               mCachePath(NULL), mIsContextSlotNotAvail(false),
               mpExtSymbolLookupFn(NULL), mpExtSymbolLookupFnContext(NULL) {
      Compiler::GlobalInitialization();

      mSourceList[0] = NULL;
      mSourceList[1] = NULL;
    }

    ~Script();

    int addSourceBC(size_t idx,
                    char const *resName,
                    const char *bitcode,
                    size_t bitcodeSize,
                    unsigned long flags);

    int addSourceModule(size_t idx,
                        llvm::Module *module,
                        unsigned long flags);

    int addSourceFile(size_t idx,
                      char const *path,
                      unsigned long flags);

    int prepareExecutable(char const *cachePath, unsigned long flags);

    char const *getCompilerErrorMessage();

    void *lookup(const char *name);


    size_t getExportVarCount() const;

    size_t getExportFuncCount() const;

    size_t getPragmaCount() const;

    size_t getFuncCount() const;

    size_t getObjectSlotCount() const;

    void getExportVarList(size_t size, void **list);

    void getExportFuncList(size_t size, void **list);

    void getPragmaList(size_t size,
                       char const **keyList,
                       char const **valueList);

    void getFuncInfoList(size_t size, FuncInfo *list);

    void getObjectSlotList(size_t size, uint32_t *list);


    int registerSymbolCallback(BCCSymbolLookupFn pFn, void *pContext);

    char *getContext();


    void setError(int error) {
      if (mErrorCode == BCC_NO_ERROR && error != BCC_NO_ERROR) {
        mErrorCode = error;
      }
    }

    int getError() {
      int result = mErrorCode;
      mErrorCode = BCC_NO_ERROR;
      return result;
    }

  private:
#if USE_CACHE
    int internalLoadCache();
#endif
    int internalCompile();

  };

} // namespace bcc

#endif // BCC_SCRIPT_H