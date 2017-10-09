//
// AfterPasswords.h
//
#ifndef __AFTER_PASSWORDS_H__
#define __AFTER_PASSWORDS_H__

#ifndef WIFI_PASSWORDS
#define WIFI_PASSWORDS {  }
#endif

#ifndef CLEAR_WIFI_PASSWORDS
#define CLEAR_WIFI_PASSWORDS false
#endif

WiFiPassword wifiPasswords[] = WIFI_PASSWORDS;

inline void setCredentials()
{
    NetworkManager::theInstance->setClearCredentials(CLEAR_WIFI_PASSWORDS);
    NetworkManager::theInstance->addPasswords(wifiPasswords, sizeof(wifiPasswords));
}

#endif
