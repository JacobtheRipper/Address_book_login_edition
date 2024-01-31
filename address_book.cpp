#include <iostream>
#include <cstdio>
#include <string>
#include <vector>
#include <regex>

#define MAX_INPUT_BUFFER_SIZE 256
#define CONTACTS_SAVE_FILE_NAME "address_book_database.txt"
#define USER_DATA_SAVE_FILE_NAME "user_data_database.txt"
#define TEMP_FILE_NAME "temporary.txt"

struct UserData {
  int userID;
  std::string username;
  std::string password;
};

struct Contact {
  int ID;
  std::string name;
  std::string surname;
  std::string phoneNumber;
  std::string email;
  std::string address;
};

void renderLoginMenu() {
  std::cout << "\nPress \'1\' to login \n";
  std::cout << "\nPress \'2\' to register \n";
  std::cout << "\nPress \'3\' to exit the program" << std::endl;
}

void renderUserLoggedInMenu() {
  std::cout << "\nPress \'1\' to add contact \n";
  std::cout << "Press \'2\' to search contacts by name \n";
  std::cout << "Press \'3\' to search contacts by surname \n";
  std::cout << "Press \'4\' to show the list of contacts \n";
  std::cout << "Press \'5\' to delete a contact\n";
  std::cout << "Press \'6\' to edit a contact\n";
  std::cout << "Press \'7\' to change password\n";
  std::cout << "Press \'8\' to logout" << std::endl;
}

void renderEditMenu() {
  std::cout << "\nPress \'1\' to edit contact name\n";
  std::cout << "Press \'2\' to edit contact surname\n";
  std::cout << "Press \'3\' to edit contact phone number\n";
  std::cout << "Press \'4\' to edit contact email address\n";
  std::cout << "Press \'5\' to edit contact address\n";
  std::cout << "Press \'6\' to return to main menu" << std::endl;
}

std::string readLine() {
  std::string userInput = std::string();
  std::getline(std::cin, userInput);
  return userInput;
}

char readCharacter() {
  std::string userInput = std::string();
  bool validatingInput = true;
  char inputChar = NULL;

  std::getline(std::cin, userInput);

  if (userInput.length() == 1) {
    inputChar = userInput.front();
    validatingInput = false;
  }
  else {
    std::cout << "Incorrect input - a single character is expected." << std::endl;
  }

  return inputChar;
}

void printContactInfo(const Contact &contactData) {
  std::cout << "ID: " << contactData.ID << '\n';
  std::cout << "Name: " << contactData.name << '\n';
  std::cout << "Surname: " << contactData.surname << '\n';
  std::cout << "Phone Number: " << contactData.phoneNumber << '\n';
  std::cout << "E-Mail: " << contactData.email << '\n';
  std::cout << "Address: " << contactData.address << std::endl;
}

void printEditedContactInfo(const Contact &contactData) {
  std::cout << "\nNew contact data for contact with ID: " << contactData.ID << '\n';
  printContactInfo(contactData);
  std::cout << "\nReturning to editing menu.\n";
}

// Remember to release memory allocated by the array of strings
std::string *splitContactDataByVerticalBars(const std::string &contactDataInFile) {
  int arrayIterator = 0;
  const int NUMBER_OF_MEMBERS_IN_DATA_STRUCTURE = 6;
  char delimiterSign = '|';
  std::string *contactDataAfterSplitting = new std::string[NUMBER_OF_MEMBERS_IN_DATA_STRUCTURE];
  std::size_t startOfWord = 0, endOfWord = contactDataInFile.find(delimiterSign);

  while (endOfWord != std::string::npos) {
    contactDataAfterSplitting[arrayIterator] = contactDataInFile.substr(startOfWord, endOfWord - startOfWord);
    startOfWord = endOfWord + 1;
    endOfWord = contactDataInFile.find(delimiterSign, startOfWord);
    ++arrayIterator;
  }
  contactDataAfterSplitting[NUMBER_OF_MEMBERS_IN_DATA_STRUCTURE - 1] += contactDataInFile.substr(startOfWord, endOfWord - startOfWord);

  return contactDataAfterSplitting;
}

std::string saveContactToSingleStringInNewFormat(const Contact &contactData) {
  std::string output = std::string();
  char delimiterSign = '|';

  output.append(std::to_string(contactData.ID));
  output.push_back(delimiterSign);
  output.append(contactData.name);
  output.push_back(delimiterSign);
  output.append(contactData.surname);
  output.push_back(delimiterSign);
  output.append(contactData.phoneNumber);
  output.push_back(delimiterSign);
  output.append(contactData.email);
  output.push_back(delimiterSign);
  output.append(contactData.address);
  output.push_back('\n');

  return output;
}

void saveContactToFile(const Contact &contactData) {
  FILE *filePointer = std::fopen(CONTACTS_SAVE_FILE_NAME, "a");
  std::string contactDataToSave;

  if (filePointer == NULL) {
    std::cout << "\nError opening file." << std::endl;
    return;
  }

  contactDataToSave = saveContactToSingleStringInNewFormat(contactData);

  std::fputs(contactDataToSave.c_str(), filePointer);
  std::fclose(filePointer);
}

int readContactsFromFile(std::vector<Contact>& contacts) {
  FILE *filePointer = std::fopen(CONTACTS_SAVE_FILE_NAME, "r");
  Contact contactExtractedFromFile;
  std::string fileLine;
  std::string *pointerToContactData = nullptr;
  int highestContactID = 0;
  char buffer[MAX_INPUT_BUFFER_SIZE];

  if (filePointer == NULL) {
    std::fclose(filePointer);
    return highestContactID;
  }

  while (std::fgets(buffer, MAX_INPUT_BUFFER_SIZE, filePointer)) {
    fileLine = std::string(buffer);
    pointerToContactData = splitContactDataByVerticalBars(fileLine);

    contactExtractedFromFile.ID = std::stoi(pointerToContactData[0]);
    contactExtractedFromFile.name = pointerToContactData[1];
    contactExtractedFromFile.surname = pointerToContactData[2];
    contactExtractedFromFile.phoneNumber = pointerToContactData[3];
    contactExtractedFromFile.email = pointerToContactData[4];
    contactExtractedFromFile.address = pointerToContactData[5];

    contacts.push_back(contactExtractedFromFile);
    highestContactID = contactExtractedFromFile.ID;
    delete[] pointerToContactData;
  }

  std::fclose(filePointer);
  return highestContactID;
}

void removeContactFromFile(int targetContactID) {
  FILE *filePointer = std::fopen(CONTACTS_SAVE_FILE_NAME, "r");
  FILE *temporaryFilePointer = std::fopen(TEMP_FILE_NAME, "a");
  std::string fileLine;
  std::string *pointerToContactData = nullptr;
  char buffer[MAX_INPUT_BUFFER_SIZE];

  if (filePointer == NULL || temporaryFilePointer == NULL) {
    std::cout << "\nError opening file." << std::endl;
    std::fclose(filePointer);
    std::fclose(temporaryFilePointer);
    return;
  }

  while (std::fgets(buffer, MAX_INPUT_BUFFER_SIZE, filePointer)) {
    fileLine = std::string(buffer);
    pointerToContactData = splitContactDataByVerticalBars(fileLine);

    if (targetContactID != std::stoi(pointerToContactData[0])) {
      std::fputs(fileLine.c_str(), temporaryFilePointer);
    }

    delete[] pointerToContactData;
  }

  std::fclose(filePointer);
  std::fclose(temporaryFilePointer);

  std::remove(CONTACTS_SAVE_FILE_NAME);
  std::rename(TEMP_FILE_NAME, CONTACTS_SAVE_FILE_NAME);
}

void editContactInFile(const Contact &editedContactData) {
  FILE *filePointer = std::fopen(CONTACTS_SAVE_FILE_NAME, "r");
  FILE *temporaryFilePointer = std::fopen(TEMP_FILE_NAME, "a");
  std::string fileLine;
  std::string *pointerToContactData = nullptr;
  char buffer[MAX_INPUT_BUFFER_SIZE];

  if (filePointer == NULL || temporaryFilePointer == NULL) {
    std::cout << "\nError opening file." << std::endl;
    std::fclose(filePointer);
    std::fclose(temporaryFilePointer);
    return;
  }

  while (std::fgets(buffer, MAX_INPUT_BUFFER_SIZE, filePointer)) {
    fileLine = std::string(buffer);
    pointerToContactData = splitContactDataByVerticalBars(fileLine);

    if (editedContactData.ID == std::stoi(pointerToContactData[0])) {
      std::fputs(saveContactToSingleStringInNewFormat(editedContactData).c_str(), temporaryFilePointer);
    }
    else {
      std::fputs(fileLine.c_str(), temporaryFilePointer);
    }

    delete[] pointerToContactData;
  }

  std::fclose(filePointer);
  std::fclose(temporaryFilePointer);

  std::remove(CONTACTS_SAVE_FILE_NAME);
  std::rename(TEMP_FILE_NAME, CONTACTS_SAVE_FILE_NAME);
}

void findContactByName(std::vector<Contact>& contacts) {
  std::string nameToFind;
  bool matchingNameFound = false;

  std::cout << "\nEnter name to be searched." << std::endl;
  nameToFind = readLine();

  for (auto contact: contacts) {
    if (contact.name == nameToFind) {
      printContactInfo(contact);
      std::cout << '\n';
      matchingNameFound = true;
    }
  }

  if (!matchingNameFound)
    std::cout << "\nContact not found." << std::endl;
}

void findContactBySurname(std::vector<Contact>& contacts) {
  std::string surnameToFind;
  bool matchingSurnameFound = false;

  std::cout << "\nEnter surname to be searched." << std::endl;
  surnameToFind = readLine();

  for (auto contact: contacts) {
    if (contact.surname == surnameToFind) {
      printContactInfo(contact);
      std::cout << '\n';
      matchingSurnameFound = true;
    }
  }

  if (!matchingSurnameFound)
    std::cout << "\nContact not found." << std::endl;
}

int findContactByID(std::vector<Contact>& contacts) {
  int targetID;
  bool matchingIDFound = false;

  std::cout << "\nTo search for a contact enter contact's ID number." << std::endl;
  targetID = std::stoi(readLine());

  for (auto contact: contacts) {
    if (contact.ID == targetID) {
      printContactInfo(contact);
      std::cout << '\n';
      matchingIDFound = true;
      return targetID;
    }
  }

  if (!matchingIDFound) {
    std::cout << "\nContact not found." << std::endl;
  }
  
  return 0;
}

int getHighestContactID(std::vector<Contact>& contacts) {
  return contacts.back().ID;
}

int getContactIndexByID(std::vector<Contact>& contacts, int targetContactID) {
  for (int i = 0; i < contacts.size(); i++) {
    if (contacts.at(i).ID == targetContactID) {
      return i;
    }
  }
  
  return contacts.size();
}

void listContacts(std::vector<Contact>& contacts, int highestContactID) {
  if (highestContactID == 0) {
    std::cout << "\nNo contacts in the database." << std::endl;
    return;
  }
  for (auto contact: contacts) {
    printContactInfo(contact);
    std::cout << '\n';
  }
}

int addContact(std::vector<Contact>& contacts, int highestContactID) {
  std::regex emailRegex("(\\w+)(\\._)?(\\w*)@(\\w+)(\\.(\\w+))+");
  Contact contactToBeAdded;
  std::string contactName, contactSurname, contactEmail, contactAddress, contactPhoneNumber;

  std::cout << "\nEnter name. Press \'Enter\' to continue.\n";
  contactName = readLine();
  contactToBeAdded.name = contactName;

  std::cout << "\nEnter surname. Press \'Enter\' to continue.\n";
  contactSurname = readLine();
  contactToBeAdded.surname = contactSurname;

  std::cout << "\nEnter phone number. Press \'Enter\' to continue.\n";
  contactPhoneNumber = readLine();
  contactToBeAdded.phoneNumber = contactPhoneNumber;

  do {
    std::cout << "\nEnter email address. Press \'Enter\' to continue.\n";
    contactEmail = readLine();

    if (std::regex_match(contactEmail, emailRegex))
      contactToBeAdded.email = contactEmail;
    else
      std::cout << "\nIncorrect input - email adress has to follow a pattern such as \'test@domain.com\'. Please try again." << std::endl;
  } while (!std::regex_match(contactEmail, emailRegex));

  std::cout << "\nEnter address. Press \'Enter\' to continue.\n";
  contactAddress = readLine();
  contactToBeAdded.address = contactAddress;

  contactToBeAdded.ID = highestContactID + 1;

  saveContactToFile(contactToBeAdded);

  std::cout << "\nContact added." << std::endl;
  contacts.push_back(contactToBeAdded);
  return ++highestContactID;
}

int deleteContact(std::vector<Contact>& contacts) {
  int contactID = findContactByID(contacts), contactIndex;
  char userInput = NULL;

  if (contactID == 0) {
    return contactID;
  }

  contactIndex = getContactIndexByID(contacts, contactID);

  std::cout << "\nDo you wish to delete the contact? If yes enter \'y\' to continue." << std::endl;
  userInput = readCharacter();

  if (userInput != 'y') {
    std::cout << "\nOperation aborted. Returning to main menu\n";
    return getHighestContactID(contacts);
  }

  contacts.erase(contacts.begin() + contactIndex);
  removeContactFromFile(contactID);

  return getHighestContactID(contacts);
}

void editContact(std::vector<Contact>& contacts) {
  const char EDIT_MENU_OPTION_EDIT_NAME = '1', EDIT_MENU_OPTION_EDIT_SURNAME = '2';
  const char EDIT_MENU_OPTION_EDIT_PHONE_NUMBER = '3', EDIT_MENU_OPTION_EDIT_EMAIL = '4';
  const char EDIT_MENU_OPTION_EDIT_ADDRESS = '5', EDIT_MENU_OPTION_RETURN_TO_MAIN_MENU = '6';

  int contactID = findContactByID(contacts), contactIndex;
  bool exitingEditMenu;
  char editMenuOption;

  if (contactID == 0) {
    return;
  }

  contactIndex = getContactIndexByID(contacts, contactID);
  exitingEditMenu = false;
  editMenuOption = NULL;
  std::system("pause");
  std::system("cls");

  while (!exitingEditMenu) {
    renderEditMenu();
    editMenuOption = readCharacter();

    switch (editMenuOption) {
    case EDIT_MENU_OPTION_EDIT_NAME:
      std::cout << "\nEnter name. Press \'Enter\' to continue.\n";
      contacts[contactIndex].name = readLine();
      printEditedContactInfo(contacts[contactIndex]);
      std::system("pause");
      std::system("cls");
      break;

    case EDIT_MENU_OPTION_EDIT_SURNAME:
      std::cout << "\nEnter surname. Press \'Enter\' to continue.\n";
      contacts[contactIndex].surname = readLine();
      printEditedContactInfo(contacts[contactIndex]);
      std::system("pause");
      std::system("cls");
      break;

    case EDIT_MENU_OPTION_EDIT_PHONE_NUMBER:
      std::cout << "\nEnter phone number. Press \'Enter\' to continue.\n";
      contacts[contactIndex].phoneNumber = readLine();
      printEditedContactInfo(contacts[contactIndex]);
      std::system("pause");
      std::system("cls");
      break;

    case EDIT_MENU_OPTION_EDIT_EMAIL:
      std::cout << "\nEnter email address. Press \'Enter\' to continue.\n";
      contacts[contactIndex].email = readLine();
      printEditedContactInfo(contacts[contactIndex]);
      std::system("pause");
      std::system("cls");
      break;

    case EDIT_MENU_OPTION_EDIT_ADDRESS:
      std::cout << "\nEnter address. Press \'Enter\' to continue.\n";
      contacts[contactIndex].address = readLine();
      printEditedContactInfo(contacts[contactIndex]);
      std::system("pause");
      std::system("cls");
      break;

    case EDIT_MENU_OPTION_RETURN_TO_MAIN_MENU:
      std::cout << "\nReturning to main menu.\n";
      exitingEditMenu = true;
      break;

    default:
      std::cout << "\nIncorrect input. Please try again.\n";
      std::system("pause");
      std::system("cls");
      break;
    }
  }

  editContactInFile(contacts[contactIndex]);
  return;
}

void switchToUserLoggedInMenu(int loggedInUserID) {
  if (loggedInUserID == 0) {
    std::cout << "\nUser is not logged in.\n";
    return;
  }
  
  const char USER_LOGGED_IN_MENU_OPTION_ADD_CONTACTS = '1', USER_LOGGED_IN_MENU_OPTION_SEARCH_BY_NAME = '2';
  const char USER_LOGGED_IN_MENU_OPTION_SEARCH_BY_SURNAME = '3', USER_LOGGED_IN_MENU_OPTION_LIST_CONTACTS = '4';
  const char USER_LOGGED_IN_MENU_OPTION_DELETE_CONTACT = '5', USER_LOGGED_IN_MENU_OPTION_EDIT_CONTACT = '6';
  const char USER_LOGGED_IN_MENU_OPTION_CHANGE_PASSWORD = '7', USER_LOGGED_IN_MENU_OPTION_LOGOUT = '8';

  std::vector<Contact> contacts;
  int highestContactID = readContactsFromFile(contacts);

  int userLoggedInMenuOption;
  bool exitingUserLoggedInMenu = false;

  std::system("cls");
  std::cout << "Hello, user.\n";

  while (!exitingUserLoggedInMenu) {
    renderUserLoggedInMenu();
    userLoggedInMenuOption = readCharacter();

    switch (userLoggedInMenuOption) {
    case USER_LOGGED_IN_MENU_OPTION_ADD_CONTACTS:
      highestContactID = addContact(contacts, highestContactID);
      printContactInfo(contacts.back());
      std::system("pause");
      std::system("cls");
      break;

    case USER_LOGGED_IN_MENU_OPTION_SEARCH_BY_NAME:
      findContactByName(contacts);
      std::system("pause");
      std::system("cls");
      break;

    case USER_LOGGED_IN_MENU_OPTION_SEARCH_BY_SURNAME:
      findContactBySurname(contacts);
      std::system("pause");
      std::system("cls");
      break;

    case USER_LOGGED_IN_MENU_OPTION_LIST_CONTACTS:
      listContacts(contacts, highestContactID);
      std::system("pause");
      std::system("cls");
      break;

    case USER_LOGGED_IN_MENU_OPTION_DELETE_CONTACT:
      highestContactID = deleteContact(contacts);
      std::system("pause");
      std::system("cls");
      break;

    case USER_LOGGED_IN_MENU_OPTION_EDIT_CONTACT:
      editContact(contacts);
      std::system("pause");
      std::system("cls");
      break;
    
    case USER_LOGGED_IN_MENU_OPTION_CHANGE_PASSWORD:
      // TODO: Implement functionality. 
      //changeUserPassword();
      std::cout << "\nWork in progress. \n";
      std::system("pause");
      std::system("cls");
      break;
    
    case USER_LOGGED_IN_MENU_OPTION_LOGOUT:
      std::cout << "\nSuccessfully logged out. Have a good day, user!\n";
      std::system("pause");
      std::system("cls");
      exitingUserLoggedInMenu = true;
      break;

    default:
      std::cout << "\nIncorrect input. Please try again.\n";
      std::system("pause");
      std::system("cls");
      break;
    }
  }
}

int main() {
  const char LOGIN_MENU_OPTION_USER_LOGIN = '1', LOGIN_MENU_OPTION_USER_REGISTER = '2';
  const char LOGIN_MENU_OPTION_EXIT_PROGRAM = '3';

  std::vector<UserData> users;
  // TODO: Implement file reading and saving for user data
  //int highestUserID = readUsersFromFile(users);
  int userID = 0;

  int loginMenuOption;
  bool exitingLoginMenu = false, userLoggedIn = false;

  std::system("cls");
  std::cout << "Hello, user.\n";

  while (!exitingLoginMenu) {
    renderLoginMenu();
    loginMenuOption = readCharacter();

    switch (loginMenuOption) {
    case LOGIN_MENU_OPTION_USER_LOGIN:
      // TODO: Implement functionality. Remove line printing after testing.
      //userID = loginUser();
      std::cout << "\nSuccessfully logged in. Have good day, user!\n";
      std::system("pause");
      std::system("cls");

      if (userLoggedIn) {
        switchToUserLoggedInMenu(userID);
      }
      break;

    case LOGIN_MENU_OPTION_USER_REGISTER:
      // TODO: Implement functionality. Remove line printing after testing.
      //highestUserID = RegisterUser();
      std::cout << "\nAccount created.\n";
      std::system("pause");
      std::system("cls");
      break;

    case LOGIN_MENU_OPTION_EXIT_PROGRAM:
      exitingLoginMenu = true;
      break;

    default:
      std::cout << "\nIncorrect input. Please try again.\n";
      std::system("pause");
      std::system("cls");
      break;
    }
  }

  std::cout << "\nHave a good day, user!" << std::endl;
  std::system("pause");
}
