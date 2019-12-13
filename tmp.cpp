#include <bits/stdc++.h>
using namespace std;

class Asteroid;
struct SpaceShip {
    virtual void CollideWith(Asteroid &inAsteroid);
};
struct ApolloSpacecraft : public SpaceShip {
    void CollideWith(Asteroid &inAsteroid) override;
};

class Asteroid {
  public:
    virtual void CollideWith(SpaceShip &) { std::cout << "Asteroid hit a SpaceShip\n"; }
    virtual void CollideWith(ApolloSpacecraft &) {
        std::cout << "Asteroid hit an ApolloSpacecraft\n";
    }
};

class ExplodingAsteroid : public Asteroid {
  public:
    void CollideWith(SpaceShip &) override { std::cout << "ExplodingAsteroid hit a SpaceShip\n"; }
    void CollideWith(ApolloSpacecraft &) override {
        std::cout << "ExplodingAsteroid hit an ApolloSpacecraft\n";
    }
};

void SpaceShip::CollideWith(Asteroid &inAsteroid) { inAsteroid.CollideWith(*this); }

void ApolloSpacecraft::CollideWith(class Asteroid &inAsteroid) { inAsteroid.CollideWith(*this); }

int main() {
    {
        cout << "============================================================================\n";
        Asteroid theAsteroid;
        SpaceShip theSpaceShip;
        ApolloSpacecraft theApolloSpacecraft;
        theAsteroid.CollideWith(theSpaceShip);
        theAsteroid.CollideWith(theApolloSpacecraft);
    }

    {
        cout << "============================================================================\n";
        SpaceShip theSpaceShip;
        ApolloSpacecraft theApolloSpacecraft;
        ExplodingAsteroid theExplodingAsteroid;

        theExplodingAsteroid.CollideWith(theSpaceShip);
        theExplodingAsteroid.CollideWith(theApolloSpacecraft);
    }

    {
        cout << "============================================================================\n";
        SpaceShip theSpaceShip;
        ApolloSpacecraft theApolloSpacecraft;
        ExplodingAsteroid theExplodingAsteroid;

        Asteroid &theAsteroidReference = theExplodingAsteroid;
        theAsteroidReference.CollideWith(theSpaceShip);
        theAsteroidReference.CollideWith(theApolloSpacecraft);
    }

    {
        cout << "============================================================================\n";
        Asteroid theAsteroid;
        ApolloSpacecraft theApolloSpacecraft;
        ExplodingAsteroid theExplodingAsteroid;
        Asteroid &theAsteroidReference = theExplodingAsteroid;

        SpaceShip &theSpaceShipReference = theApolloSpacecraft;
        theAsteroid.CollideWith(theSpaceShipReference);
        theAsteroidReference.CollideWith(theSpaceShipReference);

        cout << endl;
        theSpaceShipReference.CollideWith(theAsteroid);
        theSpaceShipReference.CollideWith(theAsteroidReference);
    }
}