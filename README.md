# Algorithms 2 Project: Zombie AI

# The final result:

<table> <tr> <td><img src="./readmeAssets/fleeing.gif" width="400"/></td> <td><img src="./readmeAssets/combat.gif" width="400"/></td> </tr> </table> <p align="center"> <img src="./readmeAssets/exploration.gif" width="400"/> </p>

---

# Explaination:

## Decision Making
 
![Decision making tree](readmeAssets/decision_making.png)   

---
 
## Enemy Handling
 
![Enemy handling code](readmeAssets/enemy_handling_code.png)
 
![Enemy handling diagram](readmeAssets/enemy_handling_diagram.png)
 
**Assess Threat (Service)** - the glue
 
*Manages the transition between FLEE and COMBAT by setting the bShouldFlee flag*
 
Information needed to set the **bShouldFlee** flag:
- Threat Actor
- Threat Speed
- Threats Count
- Health/Stamina

---

## Enemy Handling - Fleeing and Combat
 
### Fleeing
 
Uses NavMesh MoveTo
 
*The conditions to FLEE:*
1. No weapon / no ammo on weapon
2. Health too low
3. Too many Threats nearby

### Combat

Uses Face and Flee Steering Behaviors

*FSM is the perfect fit for Face and Fire / Back off when cornered / Re-aim when zombie moves*

![Combat transition code](readmeAssets/combat_code.png)

FSM is Built and Started in the Combat Task Enter.
FSM is Ticked in Combat Task Tick.
FSM is stopped in Combat Task Abort and End.

---

Combat States are assigned Transitions which constantly check the distance between Survivor and Threat.

---

## Inventory

### Looting

Memory

![Memory removal code](readmeAssets/looting_memory_code.png)

Priority

![Priority write code](readmeAssets/looting_priority_code.png)

![Item priority switch-case code](readmeAssets/looting_priority_switchCode.png)

### Using/Dropping an Item

- Eat Food when Stamina ≤ 0.3
- Use Medkit when Health ≤ 0.3

![Use item behavior tree](readmeAssets/use_item_behavior_tree.png)
 
Drop used Items
 
![Drop used items code](readmeAssets/drop_items_code.png)
 
---

## Movement - Steering
 
1. **Combat**
   - Uses Face Steering for Engage
   - Uses Flee Steering for Reposition
2. **Reacting to Damage**
   - "Scans" by rotating around itself
3. **Fleeing is interesting**
   - FINDS a flee point by selecting "Best one" in a radius
   - Furthest point from TargetThreat(Zombie) Actor
4. **Fleeing from Purge Zone**
   - Reuses same logic - runs away from PurgeToAvoid Actor

![Flee settings panel](readmeAssets/steering_flee_settings.png)

![Escape purge behavior tree](readmeAssets/steering_escape_purge_tree.png)

---

## Movement - Exploration

### Investigating a House

*Why Investigate a House?*
- Push a House to the Array of remembered houses
- But more importantly, push Items to the Array of remembered items
*What does Refresh World Memory do?*
1. Removes old/stagnant memory for zombies, items and houses
2. Updates Houses visited flag
3. Writes to Blackboard
![Known house write code](readmeAssets/exploration_house_code.png)

![Investigate house behavior tree](readmeAssets/exploration_house_tree.png)

### Discovering (Lowest Priority)

![Explore settings panel](readmeAssets/exploration_explore_settings.png)