#include "pch.h"
#include "ReactPackageProvider.h"
#include "RNFileViewer.h"
#include "ReactPackageProvider.g.cpp"

using namespace winrt::Microsoft::ReactNative;

namespace winrt::RNFileViewerCPP::implementation
{
    void ReactPackageProvider::CreatePackage(IReactPackageBuilder const& packageBuilder) noexcept
    {
        AddAttributedModules(packageBuilder);
    }
} // winrt::RNFileViewerCPP::implementation
