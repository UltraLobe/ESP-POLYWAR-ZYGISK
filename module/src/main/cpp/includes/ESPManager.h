struct enemy_t {
    void* object;
};

class ESPManager {
public:
    std::vector<enemy_t*> enemies;

    ESPManager() = default;

    bool isEnemyPresent(void* enemyObject) const {
        return std::find_if(enemies.begin(), enemies.end(), [&](const enemy_t* enemy) {
            return enemy->object == enemyObject;
        }) != enemies.end();
    }

    void tryAddEnemy(void* enemyObject) {
        if (isEnemyPresent(enemyObject)) {
            return;
        }
        enemies.push_back(new enemy_t{ enemyObject });
    }

    void removeEnemyGivenObject(void* enemyObject) {
        enemies.erase(std::remove_if(enemies.begin(), enemies.end(), [&](const enemy_t* enemy) {
            return enemy->object == enemyObject;
        }), enemies.end());
    }
};

std::unique_ptr<ESPManager> espManager = std::make_unique<ESPManager>();

