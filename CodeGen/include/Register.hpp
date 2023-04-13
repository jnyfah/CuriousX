#include <iostream>
#include <vector>
#include <bitset>
#include <unordered_map>

class RegisterAllocator {
public:
    RegisterAllocator() {
        _free_registers.set();
    }

    // Allocate a free register
    int alloc_register() {
        int reg = _findfirst();
        if (reg == -1) {
            // All registers are in use, spill one to memory
            reg = spill_register();
        } else {
            _free_registers.reset(reg);
        }
        return reg;
    }

    // Free an allocated register
    void free_register(int reg) {
        if (_spilled_registers.count(reg) > 0) {
            // Register was spilled, reload from memory
            reload_register(reg);
            _spilled_registers.erase(reg);
        } else {
            _free_registers.set(reg);
        }
    }

    // Set all registers as available
    void free_all_registers() {
        _free_registers.set();
        _spilled_registers.clear();
    }

    bool check_free_registers() {
    return _free_registers.any();
    }



private:
    std::bitset<10> _free_registers; // Available registers
    std::unordered_map<int, int> _spilled_registers; // Spilled registers (register -> memory location)
    std::vector<int> _memory; // Memory to store spilled registers
    

// Spill a register to memory and return the memory location
    int spill_register() {
        int reg = _Find_first_unset(); 
        int mem_location = _memory.size();
        _memory.push_back(reg);
        _spilled_registers[reg] = mem_location;
        _free_registers.reset(reg);
        return reg;
    }
    // Reload a spilled register from memory
    void reload_register(int reg) {
        // Retrieve the spilled register value from memory and store it back in the register
        int mem_loc = _spilled_registers[reg];
        int value = _memory[mem_loc];
        _free_registers.reset(reg);
        _memory.erase(_memory.begin() + mem_loc);
        _spilled_registers.erase(reg);
        _free_registers.set(value);
    }

    unsigned long _findfirst()
    {
        for (size_t i = 0; i < _free_registers.size(); ++i) {
            if (_free_registers.test(i)) {
            return static_cast<unsigned long>(i);
            }
        }
    return static_cast<unsigned long>(-1);
    }

    unsigned long _Find_first_unset() {
        for (size_t i = 0; i < _free_registers.size(); ++i) {
            if (!_free_registers.test(i)) {
                return static_cast<unsigned long>(i);
            }
        }
        return static_cast<unsigned long>(-1);
    }

};
