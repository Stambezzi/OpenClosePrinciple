#include <iostream>
#include <vector>

enum EColor{ eRed, eGreen, eBlue };
enum ESize{ eS, eM, eL, eXL };

std::string EColorToStirng(EColor color)
{
    std::string result;
    switch(color)
    {
    case EColor::eRed:
        result = "Red";
        break;
    case EColor::eGreen:
        result = "Green";
        break;
    case EColor::eBlue:
        result = "Blue";
        break;
    default:
        break;
    }
    return result;
}

std::string ESizeToStirng(ESize size)
{
    std::string result;
    switch (size)
    {
    case ESize::eS:
        result = "Small";
        break;
    case ESize::eM:
        result = "Medium";
        break;
    case ESize::eL:
        result = "Large";
        break;
    case ESize::eXL:
        result = "Xtra Large";
        break;
    default:
        break;
    }
    return result;
}

class Product
{
public:
    Product(const std::string& type, EColor color, ESize size) 
        : fType(type)
    {
        fColor = color;
        fSize = size;
    }

    std::string GetType() { return fType; }
    EColor      GetColor() { return fColor; }
    ESize       GetSize() { return fSize; }

private:
    std::string fType;
    EColor      fColor;
    ESize       fSize;
};

template <typename T>
class MultipleSpecifications;

template <typename T>
class Specification
{
public:
    virtual bool IsSatisfied(T item) = 0;

    MultipleSpecifications<T> operator&& (Specification <T>& other)
    {
        return MultipleSpecifications<T>(this, &other);
    }
};

class SpecByColor : public Specification<Product>
{
public:
    SpecByColor(EColor color) { fColor = color; }

    virtual bool IsSatisfied(Product item) override
    {
        return item.GetColor() == fColor;
    }

private:
    EColor fColor;
};

class SpecBySize : public Specification<Product>
{
public:
    SpecBySize(ESize size) { fSize = size; }

    virtual bool IsSatisfied(Product item) override
    {
        return item.GetSize() == fSize;
    }

private:
    ESize fSize;
};

class SpecByType: public Specification<Product>
{
public:
    SpecByType(const std::string& type) { fType = type; }

    virtual bool IsSatisfied(Product item) override
    {
        return item.GetType() == fType;
    }

private:
    std::string fType;
};

template <typename T>
class MultipleSpecifications : public Specification<T>
{
public:
    MultipleSpecifications(Specification<T>* first, Specification<T>* second) 
    {
        fSpecifications.push_back(first);
        fSpecifications.push_back(second);
    }

    MultipleSpecifications(MultipleSpecifications<T>* current, Specification<T>* append)
    {
        fSpecifications = current->fSpecifications;
        fSpecifications.push_back(append);
    }

    MultipleSpecifications<T> operator&& (Specification <T>& other)
    {
        return MultipleSpecifications<T>(this, &other);
    }

    virtual bool IsSatisfied(T item) override
    {
        bool result = true;
        for (const auto& specification : fSpecifications)
        {
            if (!specification->IsSatisfied(item))
            {
                result = false;
                break;
            }
        }
        return result;
    }

private:
    std::vector<Specification<T>*> fSpecifications;
};

template <typename T>
class Filter
{
    virtual std::vector<T*> FilterIt(std::vector<T*> items, Specification<T>& spec) = 0;
    virtual std::vector<T*> FilterIt(std::vector<T*> items, MultipleSpecifications<T>& spec) = 0;
};

class ProductFilter : Filter<Product>
{
public:
    virtual std::vector<Product*> FilterIt(std::vector<Product*> items, Specification<Product>& spec) override
    {
        std::vector<Product*> filteredItems;
        for (auto& item : items)
        {
            if (spec.IsSatisfied(*item))
            {
                filteredItems.push_back(item);
            }
        }

        return filteredItems;
    }

    virtual std::vector<Product*> FilterIt(std::vector<Product*> items, MultipleSpecifications<Product>& spec) override
    {
        std::vector<Product*> filteredItems;
        for (auto& item : items)
        {
            if (spec.IsSatisfied(*item))
            {
                filteredItems.push_back(item);
            }
        }

        return filteredItems;
    }
};

int main()
{
    Product greenShirtS("Shirt", eGreen, eS);
    Product greenShirtM("Shirt", eGreen, eM);
    Product greenShirtL("Shirt", eGreen, eL);

    Product bluePantsL("Pants", eBlue, eL);
    Product bluePantsXL("Pants", eBlue, eXL);
    Product greenPantsL("Pants", eGreen, eL);

    Product blueShirtM("Shirt", eBlue, eM);
    Product blueShirtL("Shirt", eBlue, eL);

    std::vector<Product*> products;
    products.push_back(&greenShirtS);
    products.push_back(&greenShirtM);
    products.push_back(&greenShirtL);

    products.push_back(&bluePantsL);
    products.push_back(&bluePantsXL);
    products.push_back(&greenPantsL);


    products.push_back(&blueShirtM);
    products.push_back(&blueShirtL);


    SpecByColor byColor(EColor::eGreen);
    ProductFilter filter;
    std::vector<Product*>greenProducts = filter.FilterIt(products, byColor);

    SpecBySize bySize(ESize::eL);

    SpecByType byType("Pants");

    std::cout << "Filtering green products:" << std::endl;
    for (const auto& item : greenProducts)
    {
        std::cout << item->GetType() << " " << ESizeToStirng(item->GetSize()) << " " << EColorToStirng(item->GetColor()) << std::endl;
    }
    std::cout << std::endl;

    std::vector<Product*>shirts = filter.FilterIt(products, byType);

    std::cout << "Filtering pants:" << std::endl;
    for (const auto& item : shirts)
    {
        std::cout << item->GetType() << " " << ESizeToStirng(item->GetSize()) << " " << EColorToStirng(item->GetColor()) << std::endl;
    }
    std::cout << std::endl;

    auto mult = bySize && byType && byColor;

    std::vector<Product*> shirtsSizeL = filter.FilterIt(products, mult);

    std::cout << "Filtering large pants:" << std::endl;
    for (const auto& item : shirtsSizeL)
    {
        std::cout << item->GetType() << " " << ESizeToStirng(item->GetSize()) << " " << EColorToStirng(item->GetColor()) << std::endl;
    }
}
  