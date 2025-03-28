

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <random>
#include <locale>
#include <io.h>
#include <fcntl.h>
#include <Windows.h>
#include <iomanip>
#include <fstream>
#include <codecvt>
#include <filesystem>
#include <utility>
using namespace std;

//генератор шртрихкодіків
struct Barcode {
	vector<wchar_t> code;
	vector<int> digital_code;

public:
	void GenerateRandomBarcode() {
		random_device rd;
		mt19937 gen(rd());
		uniform_int_distribution<> dis(0, 9);
		uniform_int_distribution<> dis2(0, 2);

		for (int i = 0; i < 15; i++) {
			digital_code.push_back(dis(gen));
		}
		for (int i = 0; i < 20; i++) {
			if (dis2(gen) == 0) {
				code.push_back(L'\u2502');

			}
			else if (dis2(gen) == 1) {
				code.push_back(L'\u2503');
			}
			else if (dis2(gen) == 2) {
				code.push_back(L'\u2003');
			}

		}
		code.push_back(L'\u2003');

	}
	Barcode() {
		GenerateRandomBarcode();
	}

	Barcode(vector<wchar_t> code, vector<int> digital_code) : code(code), digital_code(digital_code) {}

	void printbarcode() {

		for (int i = 0; i < 5; i++) {
			for (int i = 0; i < code.size(); i++) {
				wcout << code[i];
			}
			wcout << endl;
		}


		for (int i = 0; i < digital_code.size(); i++) {
			wcout << digital_code[i];
		}
		wcout << endl;
	}

	vector<wchar_t> getVisualBarcode() const {
		return code;

	}

	vector<int> getDigitalBarcode() const {
		return digital_code;
	}

	wstring getVisualToWstring() {
		wstring str;
		for (int i = 0; i < code.size(); i++) {
			str += code[i];
		}
		return str;
	}



	bool operator==(const Barcode& other) const {
		return code == other.code && digital_code == other.digital_code;
	}






};
string WStringToString(const std::wstring& wstr) {
	if (wstr.empty()) return std::string();

	int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
	string strTo(size_needed, 0);
	WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);
	return strTo;
}

wstring StringToWString(const string& str) {
	if (str.empty()) return wstring();

	int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
	wstring wstrTo(size_needed, 0);
	MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);
	return wstrTo;
}




//продуктікі
struct Product {
	int id;
	int value;
	wstring termOfUse;
	wstring category;
	wstring name;
	double price;
	double weight;
	Barcode barcode;

	Product() : id(0), value(0), termOfUse(L""), category(L""), name(L""), price(0.0), weight(0.0) {}
	Product(int id1, int value1, wstring termOfUse1, wstring category1, wstring name1, double price1, double weight1) {
		id = id1;
		value = value1;
		termOfUse = termOfUse1;
		category = category1;
		name = name1;
		price = price1;
		weight = weight1;
	}


	void printProductInfo() {
		wcout << "*****************************" << endl;
		wcout << "ID: " << id << endl;
		wcout << "value: " << value << endl;
		wcout << "Term of Use: " << termOfUse << endl;
		wcout << "Category: " << category << endl;
		wcout << "Name: " << name << endl;
		wcout << "Price: " << price << endl;
		wcout << "Weight: " << weight << endl;
		wcout << "==========Barcode=============" << endl;
		barcode.printbarcode();
		wcout << "==============================" << endl;
		wcout << "*****************************" << endl;


	}

	vector<int> encodeBarcode() {
		vector<int> encodedValues;

		for (wchar_t symbol : barcode.getVisualToWstring()) {
			if (symbol == L'\u2502') {
				encodedValues.push_back(1);
			}
			else if (symbol == L'\u2503') {
				encodedValues.push_back(2);
			}
			else if (symbol == L'\u2003') {
				encodedValues.push_back(3);
			}
		}


		return encodedValues;

	}

	static vector<wchar_t> decodeBarcode(int encodedValue) {

		vector<wchar_t> decodedValues;
		if (encodedValue == 1) {
			decodedValues.push_back(L'\u2502');
		}
		else if (encodedValue == 2) {
			decodedValues.push_back(L'\u2503');
		}
		else if (encodedValue == 3) {
			decodedValues.push_back(L'\u2003');
		}
		return decodedValues;

	}









	bool SaveProductToFile(const Product& product, const string& filename) {
		try {

			filesystem::create_directories(filesystem::path(filename).parent_path());


			ofstream file(filename, ios::app);

			if (!file.is_open()) {
				cerr << "Failed to open file: " << filename << "" << std::endl;
				return false;
			}


			file << "ID: " << product.id << endl;
			file << "Value: " << product.value << endl;
			file << "Term of Use: " << WStringToString(product.termOfUse) << endl;
			file << "Category: " << WStringToString(product.category) << endl;
			file << "Name: " << WStringToString(product.name) << endl;
			file << "Price: " << product.price << endl;
			file << "Weight: " << product.weight << endl;
			file << "---" << endl;

			file.close();
			return true;
		}
		catch (const exception& e) {
			cerr << "Error while saving product: " << e.what() << endl;
			return false;
		}
	}






};

bool DeleteProductFromFile(const string& filename, int productId) {
	try {

		ifstream inputFile(filename);
		if (!inputFile.is_open()) {
			cerr << "Failed to open file: " << filename << "" << endl;
			return false;
		}


		vector<string> fileContents;
		string line;
		string currentProduct;
		bool isReadingProduct = false;
		bool productFound = false;

		while (getline(inputFile, line)) {
			if (line.find("ID: ") == 0) {

				if (!currentProduct.empty()) {
					fileContents.push_back(currentProduct);
				}
				currentProduct = "";
				isReadingProduct = true;


				int currentId = stoi(line.substr(4));
				if (currentId == productId) {
					productFound = true;
					isReadingProduct = false;
				}
			}


			if (isReadingProduct) {
				currentProduct += line + "\n";
			}


			if (line == "---") {
				if (isReadingProduct) {
					currentProduct += line + "\n";
					fileContents.push_back(currentProduct);
				}
				currentProduct = "";
				isReadingProduct = false;
			}
		}


		if (!currentProduct.empty() && isReadingProduct) {
			fileContents.push_back(currentProduct);
		}

		inputFile.close();


		ofstream outputFile(filename, ios::trunc);
		if (!outputFile.is_open()) {
			std::cerr << "Failed to open file: " << filename << "" << endl;
			return false;
		}

		for (const auto& product : fileContents) {
			outputFile << product;
		}

		outputFile.close();

		return productFound;
	}
	catch (const std::exception& e) {
		cerr << "Error while deleting product: " << e.what() << endl;
		return false;
	}
}

vector<Product> LoadProductFromFile(const string& filename) {
	vector<Product> products;
	try {
		std::ifstream file(filename);

		if (!file.is_open()) {
			cerr << "Failed to open file: " << filename << "" << endl;
			return products;
		}

		string line;
		Product currentProduct(0, 0, L"", L"", L"", 0.0, 0.0);
		bool isReadingProduct = false;

		while (getline(file, line)) {
			if (line.find("ID: ") == 0) {
				currentProduct.id = stoi(line.substr(4));
				isReadingProduct = true;
			}
			else if (line.find("Value: ") == 0 && isReadingProduct) {
				currentProduct.value = stoi(line.substr(7));
			}
			else if (line.find("Term of Use: ") == 0 && isReadingProduct) {
				currentProduct.termOfUse = StringToWString(line.substr(13));
			}
			else if (line.find("Category: ") == 0 && isReadingProduct) {
				currentProduct.category = StringToWString(line.substr(10));
			}
			else if (line.find("Name: ") == 0 && isReadingProduct) {
				currentProduct.name = StringToWString(line.substr(6));
			}
			else if (line.find("Price: ") == 0 && isReadingProduct) {
				currentProduct.price = stod(line.substr(7));
			}
			else if (line.find("Weight: ") == 0 && isReadingProduct) {
				currentProduct.weight = stod(line.substr(8));
			}
			else if (line == "---") {
				if (isReadingProduct) {
					products.push_back(currentProduct);
					isReadingProduct = false;
				}
			}
		}

		file.close();
		return products;
	}
	catch (const exception& e) {
		cerr << "error while loading product: " << e.what() << endl;
		return products;
	}
}










//наймити
struct employee {
	wstring fullname;
	wstring position;
	int salary;
	int number;
	double workHours;
	employee(wstring fullname, wstring position, int salary, int number, double workHours) {
		this->fullname = fullname;
		this->position = position;
		this->salary = salary;
		this->number = number;
		this->workHours = workHours;
	}


	void print() {
		wcout << "======================" << endl;
		wcout << "Fullname: " << fullname << endl;
		wcout << "Position: " << position << endl;
		wcout << "Salary: " << salary << "per hour" << endl;
		wcout << "Number: " << number << endl;
		wcout << "Work Hours: " << workHours << endl;
	}

	void addWorkHours(double hours) {
		workHours += hours;
	}


};

class SuperMarket {
	vector<Product> products;
	vector<employee> employees;
public:
	SuperMarket() {}

	void addProduct() {
		int id;
		int value;
		wstring termOfUse;
		wstring category;
		wstring name;
		double price;
		double weight;

		wcout << "Enter ID: ";
		cin >> id;
		wcout << "Enter value: ";
		cin >> value;
		wcout << "Enter Term of Use: ";
		wcin >> termOfUse;
		wcout << "Enter Category: ";
		wcin >> category;
		wcout << "Enter Name: ";
		wcin >> name;
		wcout << "Enter Price: ";
		cin >> price;
		wcout << "Enter Weight: ";
		cin >> weight;

		Product product(id, value, termOfUse, category, name, price, weight);
		products.push_back(product);

		wcout << "Product added successfully!" << endl;
	}

	void addworkHours() {
		int number;
		double hours;
		wcout << "Enter number: ";
		cin >> number;
		wcout << "Enter hours: ";
		cin >> hours;
		for (int i = 0; i < employees.size(); i++) {
			if (employees[i].number == number) {
				employees[i].addWorkHours(hours);
			}
		}
	}

	void convertHoursToSalary() {
		int number;
		double hours;
		wcout << "Enter number: ";
		cin >> number;
		wcout << "Enter hours: ";
		cin >> hours;
		for (int i = 0; i < employees.size(); i++) {
			if (employees[i].number == number) {
				wcout << "Salary: " << hours * employees[i].salary << endl;
				employees[i].workHours -= hours;
			}
		}
	}

	void printReceipt() {
			vector <pair <int, Product>> receipt;
		while (true) {
			int action;
			int count = 0;
			wcout << "**********************" << endl;
			wcout << "1 - add product to receipt" << endl;
			wcout << "2 - print receipt" << endl;
			wcout << "3 - delete product from receipt" << endl;
			wcout << "4 - exit" << endl;
			wcout << "**********************" << endl;
			wcout << "Enter action: ";
			cin >> action;
			if (action == 1) {
				
				wcout << "Enter product ID or digital barcode: ";
				int  productId;
				int value;
				cin >> productId;
				wcout << "Enter number of products: ";
				cin >> value;

				for (int i = 0; i < products.size(); i++) {
					if (products[i].id == productId) {
						
						receipt.push_back({ value, products[i] });
					}
				}
				
				wcout << "Product added to receipt!" << endl;
				
				count++;
			}
			else if (action == 2) {
				double total = 0;
				wcout << "***********Receipt***********" << endl;
				wcout << "************23/7*************" << endl;
				wcout << "Products: " << endl;
				for (int i = 0; i < receipt.size(); i++) {
					wcout << "===" << endl;
					wcout << receipt[i].second.name << " x" << receipt[i].first << " : " << receipt[i].second.price * receipt[i].first << " UAH" << endl;
					total += receipt[i].second.price * receipt[i].first;
				}
				
				wcout << "==========TOTAL=========" << endl;
				wcout << "Total: " << total << " UAH" << endl;
				wcout << "========================" << endl;
				wcout << "Thank you for your purchase!" << endl;
				wcout << "*****************************" << endl;
				
				string password = "1234";
				string inputPassword;
				while (true) {
					wcout << "********Login********" << endl;
					wcout << "1 - enter as admin" << endl;
					wcout << "2 - enter as user" << endl;
					wcout << "3 - change password" << endl;
					wcout << "0 - exit" << endl;
					wcout << "**********************" << endl;
					int action;
					cin >> action;
					switch (action)
					{
					case 1:
						wcout << "Enter password: ";
						cin >> inputPassword;
						if (inputPassword == password) {
							while (true) {
								AdminMenu();
							}
						}
						else {
							wcout << "Wrong password" << endl;
						}
						break;
					case 2:
						while (true) {
							userMenu();
						}
						break;
					case 3:
						wcout << "Enter old password: ";
						cin >> inputPassword;
						if (inputPassword == password) {
							wcout << "Enter new password: ";
							cin >> password;
						}
						break;
					case 0:
						exit(0);
						break;
					default:
						break;
					}
				}


			}
			else if (action == 3) {
				int productId;
				wcout << "Enter product ID: ";
				cin >> productId;
				for (int i = 0; i < receipt.size(); i++) {
					if (receipt[i].second.id == productId) {
						receipt.erase(receipt.begin() + i);
						wcout << "Product deleted from receipt" << endl;
					}
				}

			}
		}


	}
	void DeleteProductFromStorage(int productId) {
		string filename = "products.txt";


		bool fileDeleted = DeleteProductFromFile(filename, productId);


		auto it = remove_if(products.begin(), products.end(),
			[productId](const Product& product) { return product.id == productId; });

		products.erase(it, products.end());

		if (fileDeleted) {
			wcout << L"Product deleted successfully" << endl;
		}
		else {
			wcout << L"product not found" << endl;
		}
	}

	void SaveProductsToFile() {
		string filename = "products.txt";
		for (Product& product : products) {
			product.SaveProductToFile(product, filename);
		}
		wcout << L"Products saved to file" << endl;
	}

	void printProducts() {
		for (int i = 0; i < products.size(); i++) {
			products[i].printProductInfo();
		}
	}



	void DeleteProduct() {
		if (products.size() == 0) {
			cout << "No products found." << endl;
			return;
		}
		int id;
		wcout << "Enter ID: ";
		cin >> id;
		for (int i = 0; i < products.size(); i++) {
			if (products[i].id == id) {
				products.erase(products.begin() + i);
				break;
			}
		}

	}

	void LoadProductsFromFile() {
		string filename = "products.txt";
		products = LoadProductFromFile(filename);
		wcout << L"Products loaded from file" << endl;
	}

	void SearchProduct() {
		int action;
		wcout << "*********Search Product*********" << endl;
		wcout << "1 - search by barcode" << endl;
		wcout << "2 - search by name" << endl;
		wcout << "3 - search by id" << endl;		
		wcout << "********************************" << endl;
		cin >> action;

		if (action == 1) {
			vector<int> digital_barcode;
			long long int_digital_barcode;

			wcout << "Enter digital barcode: ";
			wcin >> int_digital_barcode;
			string string_digital_barcode = to_string(int_digital_barcode);


			for (char ch : string_digital_barcode) {
				digital_barcode.push_back(ch - '0');
			}

			for (int i = 0; i < products.size(); i++) {
				if (digital_barcode == products[i].barcode.digital_code) {
					products[i].printProductInfo();
				}

			}

		}
		else if (action == 2) {
			wstring name;
			wcout << "Enter name: ";
			wcin >> name;
			for (int i = 0; i < products.size(); i++) {
				if (products[i].name == name) {
					products[i].printProductInfo();
				}
			}
		}
		else if (action == 3) {
			int id;
			wcout << "Enter id: ";
			cin >> id;
			for (int i = 0; i < products.size(); i++) {
				if (products[i].id == id) {
					products[i].printProductInfo();
				}
			}
		}
	}

	void AddEmployee() {
		wstring fullname;
		wstring position;
		int salary;
		int number;
		double workHours;
		wcout << "Enter fullname: ";
		wcin >> fullname;
		wcout << "Enter position: ";
		wcin >> position;
		wcout << "Enter salary: ";
		cin >> salary;
		wcout << "Enter number: ";
		cin >> number;
		wcout << "Enter work hours: ";
		cin >> workHours;
		employee employee(fullname, position, salary, number, workHours);
		employees.push_back(employee);


		wcout << "Employee added successfully!" << endl;
	}

	void printEmployees() {
		for (int i = 0; i < employees.size(); i++) {
			employees[i].print();
		}
	}



	void DeleteEmployee() {
		if (employees.size() == 0) {
			wcout << "No employees found." << endl;
			return;
		}
		int number;
		wcout << "Enter number: ";
		cin >> number;
		for (int i = 0; i < employees.size(); i++) {
			if (employees[i].number == number) {
				employees.erase(employees.begin() + i);
				break;
			}
		}
	}

	void changeEmployeeInfo() {
		int number;
		int action;
		wcout << "Enter number: ";
		cin >> number;
		wcout << "1 - change position" << endl;
		wcout << "2 - change salary" << endl;
		cin >> action;
		for (int i = 0; i < employees.size(); i++) {
			if (employees[i].number == number) {
				if (action == 1) {
					wstring position;
					wcout << "Enter position: ";
					wcin >> position;
					employees[i].position = position;
				}
				else if (action == 2) {
					int salary;
					wcout << "Enter salary: ";
					cin >> salary;
					employees[i].salary = salary;
				}
			}
		}

	}

	void SearchEmployee() {
		int number;
		wcout << "Enter number: ";
		cin >> number;
		for (int i = 0; i < employees.size(); i++) {
			if (employees[i].number == number) {
				employees[i].print();
			}
		}
	}

	void ChangeProductInfo() {
		int id;
		int action;
		wcout << "Enter ID: " << endl;
		cin >> id;
		wcout << "*******Change Product Info*******" << endl;
		wcout << "1 - change value" << endl;
		wcout << "2 - change term of use" << endl;
		wcout << "3 - change category" << endl;
		wcout << "4 - change name" << endl;
		wcout << "5 - change price" << endl;
		wcout << "6 - change weight" << endl;
		wcout << "**********************************" << endl;
		cin >> action;
		for (int i = 0; i < products.size(); i++) {
			if (products[i].id == id) {
				if (action == 1) {
					int value;
					wcout << "Enter value: ";
					cin >> value;
					products[i].value = value;
				}
				else if (action == 2) {
					wstring termOfUse;
					wcout << "Enter term of use: ";
					wcin >> termOfUse;
				}
				else if (action == 3) {
					wstring category;
					wcout << "Enter category: ";
					wcin >> category;
					products[i].category = category;
				}
				else if (action == 4) {
					wstring name;
					wcout << "Enter name: ";
					wcin >> name;
					products[i].name = name;
				}
				else if (action == 5) {
					int price;
					wcout << "Enter price: ";
					cin >> price;
					products[i].price = price;
				}
				else if (action == 6) {
					int weight;
					wcout << "Enter weight: ";
					cin >> weight;
					products[i].weight = weight;
				}

			}
		}
	}

	void userMenu() {
		int action;
		wcout << "********User Menu*******" << endl;
		wcout << "1 - add product" << endl;
		wcout << "2 - add employee" << endl;
		wcout << "3 - print products" << endl;
		wcout << "4 - print employees" << endl;
		wcout << "5 - search product" << endl;
		wcout << "6 - search employee" << endl;
		wcout << "7 - convert hours to salary" << endl;
		wcout << "8 - print receipt" << endl;
		/*wcout << "9 - save Products to file" << endl;
		wcout << "10 - load products from file" << endl;*/
		wcout << "0 - exit" << endl;
		wcout << "************************" << endl;
		cin >> action;
		switch (action)

		{
		case 1:
			addProduct();
			break;
		case 2:
			AddEmployee();
			break;
		case 3:
			printProducts();
			break;
		case 4:
			printEmployees();
			break;
		case 5:
			SearchProduct();
			break;
		case 6:
			SearchEmployee();
			break;
		case 7:
			convertHoursToSalary();
			break;
		case 8:
			printReceipt();
			break;
		case 9:
			SaveProductsToFile();
			break;
		case 10:
			LoadProductsFromFile();
			break;
		case 0:
			exit(0);
			break;
		default:
			break;
		}
	}

	void AdminMenu() {
		int action;
		wcout << "********Admin Menu*******" << endl;
		wcout << "1 - add product" << endl;
		wcout << "2 - add employee" << endl;
		wcout << "3 - print products" << endl;
		wcout << "4 - print employees" << endl;
		wcout << "5 - search product" << endl;
		wcout << "6 - search employee" << endl;
		wcout << "7 - change product info" << endl;
		wcout << "8 - change employee info" << endl;
		wcout << "9 - delete product" << endl;
		wcout << "10 - delete employee" << endl;
		wcout << "11 - delete all products" << endl;
		wcout << "12 - delete all employees" << endl;
		wcout << "13 - Print receipt" << endl;
		wcout << "14 - add work hours to employee" << endl;
		wcout << "15 - convert hours to salary" << endl;
		/*wcout << "16 - save Products to file" << endl;*/
		/*wcout << "17 - load Products from file" << endl;*/
		/*wcout << "18 - delete product from file" << endl;*/
		wcout << "0 - exit" << endl;
		wcout << "************************" << endl;
		cin >> action;
		switch (action)

		{
		case 1:
			addProduct();
			break;
		case 2:
			AddEmployee();
			break;
		case 3:
			printProducts();
			break;
		case 4:
			printEmployees();
			break;
		case 5:
			SearchProduct();
			break;
		case 6:
			SearchEmployee();
			break;
		case 7:
			ChangeProductInfo();
			break;
		case 8:
			changeEmployeeInfo();
			break;
		case 9:
			DeleteProduct();
			break;
		case 10:
			DeleteEmployee();
			break;
		case 11:
			for (int i = 0; i < products.size(); i++) {
				products.erase(products.begin() + i);
			}
			break;
		case 12:
			for (int i = 0; i < employees.size(); i++) {
				employees.erase(employees.begin() + i);
			}
			break;
		case 13:
			printReceipt();
			break;
		case 14:
			addworkHours();
			break;
		case 15:
			convertHoursToSalary();
			break;
		case 16:
			SaveProductsToFile();
			break;
		case 17:
			LoadProductsFromFile();
			break;
		case 18:
			cout << "Enter ID: ";
			int id;
			cin >> id;
			DeleteProductFromStorage(id);
			break;
		case 0:
			exit(0);
			break;
		default:
			break;
		}
	}
};

class Login {
	string password = "1234";
public:
	void operator()(SuperMarket& obj) {
		string inputPassword;
		while (true) {
			wcout << "********Login********" << endl;
			wcout << "1 - enter as admin" << endl;
			wcout << "2 - enter as user" << endl;
			wcout << "3 - change password" << endl;
			wcout << "0 - exit" << endl;
			wcout << "**********************" << endl;
			wcout << "Enter action: ";
			int action;
			cin >> action;
			switch (action)
			{
			case 1:
				wcout << "Enter password: ";
				cin >> inputPassword;
				if (inputPassword == password) {
					while (true) {
						obj.AdminMenu();
					}
				}
				else {
					wcout << "Wrong password" << endl;
				}
				break;
			case 2:
				while (true) {
					obj.userMenu();
				}
				break;
			case 3:
				wcout << "Enter old password: ";
				cin >> inputPassword;
				if (inputPassword == password) {
					wcout << "Enter new password: ";
					cin >> password;
				}
				break;
			case 0:
				exit(0);
				break;
			default:
				break;
			}
		}
	}
};










int main() {
	_setmode(_fileno(stdout), _O_U16TEXT);



	SuperMarket supermarket1;

	Login login1;

	login1(supermarket1);



}