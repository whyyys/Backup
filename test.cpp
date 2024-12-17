#include <iostream>
#include <vector>
class BaseClass
{
public:
    BaseClass(int value) : data(value) {}
    virtual void printInfo()
    {
        std::cout << "BaseClass: " << data << std::endl;
    }
    void divideBy(int divisor)
    {
        data /= divisor;
    }

protected:
    int data;
};
class DerivedClass : public BaseClass
{
public:
    DerivedClass(int value) : BaseClass(value) {}
    void printInfo() override
    {
        std::cout << "DerivedClass: " << data << std::endl;
    }
};
int main()
{
    int size;
    std::cout << "Enter the number of elements: ";
    std::cin >> size;
    std::vector<BaseClass *> objects;
    for (int i = 0; i < size; ++i)
    {
        objects.push_back(new DerivedClass(i + 1));
    }
    for (int i = 0; i < size; ++i)
    { // 此处应该是 < ⽽不是 <=
        objects[i]->printInfo();
        objects[i]->divideBy(i+1); // 此处应该是 i + 1 ⽽不是 i
    }
    for (BaseClass *obj : objects)
    {
        delete obj;
    }
    return 0;
}