#pragma once

class IDealUrl
{
public:
	IDealUrl(void);
	virtual ~IDealUrl(void);

	std::string& GetMethod() { return m_strMethod; };
	void SetMethod( std::string _strMethod ) { m_strMethod = _strMethod; };

private:
	std::string m_strMethod;
};
