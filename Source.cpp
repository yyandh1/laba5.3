#include "Header.h"

#define MAX_AGE_PRED 50
#define MAX_AGE_PREY 20

class Animal
{
protected:

    Animal(pair<int,int> startCoordinates, int startDirection, int s_k)
    {   
        coordinates = startCoordinates;
        Direction = startDirection;
        k = s_k;
        countK = 0;
        age = 0;

    }
    pair<int,int> coordinates;
    int Direction;
    int k;
    int countK;
    int age;

public:
    virtual pair<int,int> Move(int x, int y) = 0;
    virtual void AddAge() = 0;
    virtual pair<int,int> GetCoord() = 0;
    virtual int GetAge() = 0;
    virtual int GetDirection() = 0;
    virtual int GetK() = 0;
    virtual bool IsValidAge() = 0;
};

class Predator : public Animal
{
private:

    int countPrey = 0;

public:

    Predator(pair<int,int> n, int s_startDirection, int s_k)
    : Animal(n, s_startDirection,  s_k) {}

    void AddAge() override
    {
        age++;
    }
    int GetK() override
    {
        return k;
    }
    bool IsValidAge() override
    {
        if(age > MAX_AGE_PRED) return false;
        return true;
    }
    int GetAge() override
    {
        return age;
    }

    pair<int,int> GetCoord() override
    {
        return coordinates;
    }

    void AddSatiety()
    {
        countPrey++;
    }
    
    int GetDirection() override
    {
        return Direction;
    }


    bool GetSatiety() 
    {
        if(countPrey == 2)
        {
            countPrey =0;
            return true;
        }
        return false;
    }

    pair<int,int> Move(int x, int y) override
    {   
        
        auto& [posX, posY] = coordinates; 

        if (countK == k)
        {
            Direction = (Direction + 1) % 4;
            countK = 0;
        }

        int newX = posX;
        int newY = posY;

        switch (Direction)
        {
            case 3: // Влево
                newX = (posX - 2 + x) % x; 
                break;

            case 0: // Вверх
                newY = (posY - 2 + y) % y; 
                break;

            case 1: // Вправо
                newX = (posX + 2) % x; 
                break;

            case 2: // Вниз
                newY = (posY + 2) % y; 
                break;
        }

        
        posX = newX;
        posY = newY;

        countK++;
        return coordinates; 
    }
};


class Prey: public Animal
{   
public:

    Prey(pair<int,int> n, int s_startDirection, int s_k)
    : Animal(n, s_startDirection,  s_k) {}

    void AddAge() override
    {
        age++;
    }

    int GetK() override
    {
        return k;
    }

    bool IsValidAge() override
    {
        if(age>MAX_AGE_PREY) return false;
        return true;
    }

    int GetAge() override
    {
       return age;
    }

    int GetDirection() override
    {
        return Direction;
    }

    pair<int,int> GetCoord() override
    {
        return coordinates;
    }
    
    pair<int,int> Move(int x, int y) override
    {   
         auto& [posX, posY] = coordinates;

        if (countK == k)
        {
            Direction = (Direction + 1) % 4;
            countK = 0;
        }

        switch (Direction)
        {
            case 3: // Влево
                posX = (posX == 0) ? (x - 1) : (posX - 1);
                break;

            case 0: // Вверх
                posY = (posY == 0) ? (y - 1) : (posY - 1);
                break;

            case 1: // Вправо
                posX = (posX == x - 1) ? 0 : (posX + 1);
                break;

            case 2: // Вниз
                posY = (posY == y - 1) ? 0 : (posY + 1);
                break;
        }
        countK++;
        return coordinates;
    }
        
    
};

class AnimalFactory
{
public:

    enum AnimalType {PREDATOR, PREY};

    static shared_ptr<Animal> createAnimal(AnimalType type, pair<int,int> coordinate, int startDirection, int s_k)
    {
        switch(type)
        {
            case PREDATOR:
                return make_shared<Predator>(coordinate, startDirection, s_k);
            case PREY:
                return make_shared<Prey>(coordinate,startDirection, s_k);
        }
    }
};


Predator* FindPredatorAtCoordinates(vector<shared_ptr<Animal>>& animals, pair<int, int>& coord) 
{
    for (const auto& animal : animals) 
    {
        // if (!animal) // Проверка на nullptr
        // {
        //    // std::cerr << "Ошибка: animal содержит nullptr!" << std::endl;
        //     continue; // Пропускаем эту итерацию
        // }

        if (auto predator = dynamic_cast<Predator*>(animal.get())) 
        {
            if (animal->GetCoord() == coord) 
            {
                //cout << "Хищник найден по координатам: (" << coord.first << ", " << coord.second << ")" << endl;
                return predator; // Возвращаем указатель на найденного хищника
            }
        }
    }
    //cout << "Хищник не найден по координатам: (" << coord.first << ", " << coord.second << ")" << endl;
    return nullptr;
}

void EatPrey(vector<shared_ptr<Animal>>& animals, pair<int, int> coord, Predator* predator) 
{
    // if (!predator) 
    // {
    //     //cout << "Не удалось съесть жертву: хищник не найден!" << endl;
    //     return; 
    // }

    predator->AddSatiety(); 

    animals.erase(remove_if(animals.begin(), animals.end(), [&coord](const shared_ptr<Animal>& animal) 
        {
            // if (!animal) // Проверка на nullptr
            // {
            //     //std::cerr << "Ошибка: animal содержит nullptr в процессе удаления!" << std::endl;
            //     return false; // Пропускаем этот объект
            // }

            return dynamic_cast<Prey*>(animal.get()) != nullptr && animal->GetCoord() == coord;
        }), animals.end());
}

void DeathFromOldAge(vector<shared_ptr<Animal>>& animals)
{
    // Удаляем всех животных, возраст которых превышает максимум
    animals.erase(
        remove_if(animals.begin(), animals.end(), [](const shared_ptr<Animal>& animal) 
        {
            // if (!animal) // Проверка на nullptr
            // {
            //     //std::cerr << "Ошибка: animal содержит nullptr в процессе удаления из-за возраста!" << std::endl;
            //     return false; // Пропускаем этот объект
            // }

            return !animal->IsValidAge(); // Если животное слишком старое
        }),
        animals.end()
    );
}

pair<vector<pair<int, int>>, vector<pair<int, int>>> 
gameLogic(const int &length, const int &width, vector<shared_ptr<Animal>>& animals) 
{
    vector<pair<int, int>> coordPred;
    vector<pair<int, int>> coordPrey;

    for (const auto& i : animals) // обработка ходов
    {
        // if (!i) // Проверка на nullptr
        // {
        //     //std::cerr << "Ошибка: обнаружен nullptr в animals при обработке ходов!" << std::endl;
        //     continue; // Пропускаем этот объект
        // }

        if (dynamic_cast<Predator*>(i.get())) 
        {
            coordPred.push_back(i->Move(width, length));
            i->AddAge();
        } 
        else if (dynamic_cast<Prey*>(i.get())) 


        {
            coordPrey.push_back(i->Move(width, length));
            i->AddAge();
        }
    }

    // Для обработки питания с удалением коордиант жертвы
    for (auto i : coordPred) 
    {
        for (auto it = coordPrey.begin(); it != coordPrey.end(); ) 
        {
            if (i == *it) 
            {
                Predator* predator = FindPredatorAtCoordinates(animals, i);
                EatPrey(animals, *it, predator); 

                if (predator && predator->GetSatiety()) 
                {   
                    auto newPred = AnimalFactory::createAnimal(
                        AnimalFactory::PREDATOR, 
                        predator->GetCoord(),
                        predator->GetDirection(),
                        predator->GetK()
                    );
                    animals.push_back(newPred); 
                }

                it = coordPrey.erase(it); 
            } 
            else 
            {
                ++it;
            }
        }
    }

    vector<shared_ptr<Animal>> newAnimals;

    for (const auto& animal : animals) //создание животных достигших определенного возраста
    {
        if (auto prey = dynamic_cast<Prey*>(animal.get())) 
        {
            if(prey->GetAge() == 5 || prey->GetAge() == 10)
            {
                auto newPrey = AnimalFactory::createAnimal(
                    AnimalFactory::PREY, 
                    prey->GetCoord(),
                    prey->GetDirection(),
                    prey->GetK()
                );
                newAnimals.push_back(newPrey);
            }
        }
    }
    animals.insert(animals.end(), newAnimals.begin(), newAnimals.end());
    DeathFromOldAge(animals); // Удаление слишком старых животных

    return {coordPred, coordPrey};
}

void PrintWindow(const int &length, const int &width, vector<shared_ptr<Animal>>& animals,  vector<vector<char>>& field) 
{
    
    auto [coordPred, coordPrey] = gameLogic(length, width, animals);
    
    for (const auto& i : animals)// обработка ходов
    {
        if (dynamic_cast<Predator*>(i.get())) 
        {
            auto [x,y] = i->GetCoord();
            field[x][y] = 'x';
        } else if (dynamic_cast<Prey*>(i.get())) 
        {
            auto [x,y] = i->GetCoord();
            field[x][y] = 'p';
        }
    }

    for (int i = 0; i < length; i++) 
    {
        for (int j = 0; j < width; j++) 
        {
            cout << field[j][i]<< " ";
        }
        cout << endl;
    }
    field.assign(length, vector<char>(width,'.'));

}


int main(void)
{   
    
    vector<shared_ptr<Animal>> animals;

    int length = 0, width = 0, numberOfMoves= 0;

    cin >> length >> width >> numberOfMoves;

    vector<vector<char>> field(length, vector<char>(width, '.'));

    int quantityPrey = 0, quantityPred = 0;
    
    cin >> quantityPrey >> quantityPred;

    int x = 0, y = 0, d = 0, k = 0;

    for(int i =0; i < quantityPrey; i++)
    {   
        cin >> x >> y >> d >> k;
        auto prey = AnimalFactory::createAnimal(AnimalFactory::PREY, {x,y},d,k);
        animals.push_back(prey);
    }

    for(int i = 0; i < quantityPred; i++)
    {
        cin >> x >> y >> d >> k;

        auto pred = AnimalFactory::createAnimal(AnimalFactory::PREDATOR, {x,y},d,k);
        animals.push_back(pred);

    }
    
    while(numberOfMoves != 0)
    {
        system("clear");
        
        PrintWindow(length, width, animals,field);

        this_thread::sleep_for(chrono::milliseconds(50));

        numberOfMoves--;

    }
    
    vector<vector<string>> field1(length, vector<string>(width, "."));

    map<pair<int,int>, int> quantityPredInCell;
    map<pair<int,int>, int> quantityPreyInCell;
    pair<int,int> key;

    for (auto& i : animals)
    {
        if (dynamic_cast<Predator*>(i.get())) 
        {   
            key = i->GetCoord();
            quantityPredInCell[key]++;

        } else if (dynamic_cast<Prey*>(i.get())) 
        {
            key = i->GetCoord();
            quantityPreyInCell[key]++;
        }
    }

    for (const auto& [key, value] : quantityPredInCell) 
    {
        field1[key.first][key.second] = "-"+to_string(value);
    }

    for (const auto& [key, value] : quantityPreyInCell) 
    {
        field1[key.first][key.second] = to_string(value);
    }

    cout<<endl;
    for (int i = 0; i < length; i++) 
    {
        for (int j = 0; j < width; j++) 
        {
            cout << field1[j][i]<< " ";
        }
        cout << endl;
    }


    return 0;
}






