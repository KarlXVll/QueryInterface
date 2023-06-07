#include <iostream>
#include <objbase.h>
#include <ObjIdl.h>
#include <initguid.h>

const CLSID CLSID_CA = { 0x00000001, 0x0000, 0x0000,{ 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46 } };
const IID IID_IX = { 0x00000001, 0x0000, 0x0000, { 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46 } };
const IID IID_IY = { 0x00000002, 0x0000, 0x0000, { 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46 } };
const IID IID_IZ = { 0x00000003, 0x0000, 0x0000, { 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46 } };


// объявление интерфейсов
interface IX {
    virtual HRESULT QueryInterface(REFIID riid, void** ppvObject) = 0;
    virtual ULONG AddRef() = 0;
    virtual ULONG Release() = 0;
};

interface IY {
    virtual HRESULT QueryInterface(REFIID riid, void** ppvObject) = 0;
    virtual ULONG AddRef() = 0;
    virtual ULONG Release() = 0;
};

interface IZ {
    virtual HRESULT QueryInterface(REFIID riid, void** ppvObject) = 0;
    virtual ULONG AddRef() = 0;
    virtual ULONG Release() = 0;
};

class IUnknownImpl : public IX, public IY, public IZ, public IUnknown {
public:
    IUnknownImpl() : m_refCount(1) {}

    // Реализация QueryInterface
    STDMETHODIMP QueryInterface(REFIID riid, void** ppv) {
        if (riid == IID_IUnknown) {
            *ppv = static_cast<IUnknown*>(this);
        }
        else if (riid == IID_IX) {
            *ppv = static_cast<IX*>(this);
        }
        else if (riid == IID_IY) {
            *ppv = static_cast<IY*>(this);
        }
        else if (riid == IID_IZ) {
            *ppv = static_cast<IZ*>(this);
        }
        else {
            *ppv = nullptr;
            return E_NOINTERFACE;
        }

        AddRef();
        return S_OK;
    }

    // Реализация AddRef
    STDMETHODIMP_(ULONG) AddRef() {
        return InterlockedIncrement(&m_refCount);
    }

    // Реализация Release
    STDMETHODIMP_(ULONG) Release() {
        ULONG refCount = InterlockedDecrement(&m_refCount);
        if (refCount == 0) {
            delete this;
        }
        return refCount;
    }

private:
    ULONG m_refCount;
};






class CA : public IX, public IY, public IUnknown{
public:
    // IX methods
    void SomeMethodX() {}

    // IY methods
    void SomeMethodY() {}

    // IZ methods
    void SomeMethodZ() {}

    // IUnknown methods
    HRESULT QueryInterface(REFIID riid, void** ppvObject) {
        if (riid == IID_IUnknown) {
            *ppvObject = static_cast<IUnknown*>(this);
        } else if (riid == IID_IX) {
            *ppvObject = static_cast<IX*>(this);
        } else if (riid == IID_IY) {
            *ppvObject = static_cast<IY*>(this);
        } else {
            *ppvObject = nullptr;
            return E_NOINTERFACE;
        }

        AddRef();
        return S_OK;
    }

    ULONG AddRef() {
        return InterlockedIncrement(&m_refCount);
    }

    ULONG Release() {
        ULONG refCount = InterlockedDecrement(&m_refCount);
        if (refCount == 0) {
            delete this;
        }
        return refCount;
    }

private:
    LONG m_refCount = 1;
};


extern "C" HRESULT __stdcall DllGetClassObject(const CLSID& clsid, const IID& iid, void** ppv) {
    if (clsid == CLSID_CA) {
        CA* pCA = new CA();
        return pCA->QueryInterface(iid, ppv);
    }

    return CLASS_E_CLASSNOTAVAILABLE;
}

extern "C" HRESULT __stdcall DllCanUnloadNow() {
    return S_FALSE;
}

// Функция CreateInstance
HRESULT CreateInstance(IUnknown** ppvObject) {
    CA* pCA = new CA();
    if (pCA == nullptr) {
        return E_OUTOFMEMORY;
    }

    HRESULT hr = pCA->QueryInterface(IID_IUnknown, (void**)ppvObject);
    pCA->Release();
    return hr;
}

int main() {
    // Инициализация COM
    CoInitialize(nullptr);

    // Создание экземпляра компонента
    IUnknown* pUnknown = nullptr;
    HRESULT hr = CreateInstance(&pUnknown);

    if (FAILED(hr)) {
        std::cout << "Failed to create instance of component" << std::endl;
        return 1;
    }

    // Запрос интерфейсов IX и IY через IUnknown
    IX* pIX = nullptr;
    IY* pIY = nullptr;

    hr = pUnknown->QueryInterface(IID_IX, (void**)&pIX);
    if (FAILED(hr)) {
        std::cout << "Failed to query IX interface" << std::endl;
        return 1;
    }

    hr = pUnknown->QueryInterface(IID_IY, (void**)&pIY);
    if (FAILED(hr)) {
        std::cout << "Failed to query IY interface" << std::endl;
        return 1;
    }

    // Запрос интерфейса IZ
    IZ* pIZ = nullptr;

    hr = pUnknown->QueryInterface(IID_IZ, (void**)&pIZ);
    if (FAILED(hr)) {
        std::cout << "Failed to query IZ interface" << std::endl;
        return 1;
    }

    // Запрос указателя на интерфейс IY через указатель на интерфейс IX
    IY* pIYthroughIX = nullptr;

    hr = pIX->QueryInterface(IID_IY, (void**)&pIYthroughIX);
    if (FAILED(hr)) {
        std::cout << "Failed to query IY interface through IX" << std::endl;
        return 1;
    }

    // Запрос интерфейса IUnknown через указатель на интерфейс IX
    IUnknown* pIXUnknown = nullptr;

    hr = pIX->QueryInterface(IID_IUnknown, (void**)&pIXUnknown);
    if (FAILED(hr)) {
        std::cout << "Failed to query IUnknown interface through IX" << std::endl;
        return 1;
    }

    // Проверка того, что два указателя на IUnknown совпадают
    if (pUnknown != pIXUnknown) {
        std::cout << "IX and IUnknown pointers are not equal" << std::endl;
        return 1;
    }

    // Освобождение ресурсов
    pUnknown->Release();
    pIX->Release();
    pIY->Release();
    pIYthroughIX->Release();
    pIZ->Release();
    pIXUnknown->Release();

    // Завершение COM
    CoUninitialize();

    return 0;
}