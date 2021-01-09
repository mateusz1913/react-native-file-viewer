#pragma once

#include "pch.h"

#include <functional>
#include <sstream>
#include <string>

#include "NativeModules.h"

namespace RNFileViewer {

    REACT_MODULE(RNFileViewer);
    struct RNFileViewer {

        winrt::Microsoft::ReactNative::ReactContext context;

        winrt::Windows::Foundation::IAsyncAction OpenAsync(std::string filePath, React::ReactPromise<void> promise) noexcept {
            auto capturedPromise = promise;
            try
            {
                winrt::hstring path = winrt::to_hstring(filePath);
                auto file = co_await winrt::Windows::Storage::StorageFile::GetFileFromPathAsync(path);
                if (file != nullptr) {
                    context.UIDispatcher().Post([&file, &capturedPromise]() -> winrt::Windows::Foundation::IAsyncAction {
                        try
                        {
                            auto success = co_await winrt::Windows::System::Launcher::LaunchFileAsync(file);
                            if (success)
                            {
                                capturedPromise.Resolve();
                            }
                            else
                            {
                                React::ReactError error{};
                                error.Message = "File open failed.";
                                capturedPromise.Reject(std::move(error));
                            }
                        }
                        catch (const std::exception& e)
                        {
                            React::ReactError error{};
                            error.Message = e.what();
                            capturedPromise.Reject(std::move(error));
                        }
                        });
                }
                else
                {
                    React::ReactError error{};
                    error.Message = "File not found.";
                    capturedPromise.Reject(std::move(error));
                }
            }
            catch (const std::exception& e)
            {
                React::ReactError error{};
                error.Message = e.what();
                capturedPromise.Reject(std::move(error));
            }
        }

        REACT_INIT(Initialize, L"Initialize");
        void Initialize(winrt::Microsoft::ReactNative::ReactContext const& reactContext) noexcept {
            context = reactContext;
        }

        REACT_METHOD(Open, L"open");
        void Open(std::string filePath, React::JSValue _, React::ReactPromise<void> promise) noexcept {
            auto asyncOp = OpenAsync(filePath, promise);
            asyncOp.Completed([promise](auto action, auto status) {
                // Here we handle any unhandled exceptions thrown during the
                // asynchronous call by rejecting the promise with the error code
                if (status == winrt::Windows::Foundation::AsyncStatus::Error)
                {
                    std::stringstream errorCode;
                    errorCode << "0x" << std::hex << action.ErrorCode() << std::endl;

                    auto error = winrt::Microsoft::ReactNative::ReactError();
                    error.Message = "HRESULT " + errorCode.str() + ": " + std::system_category().message(action.ErrorCode());
                    promise.Reject(error);
                }
            });
        }

    };
}
