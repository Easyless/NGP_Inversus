#pragma once
class InversusFramework;

class IFrameworkObject
{
protected:
	InversusFramework* framework;
	IFrameworkObject(InversusFramework* framework) : framework(framework) {};
};
