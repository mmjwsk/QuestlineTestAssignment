# QuestlineTestAssignment
Test assignment submission for Questline recruitment process authored by Michał Majewski

## Running & testing
- In order to test the solution, clone the repo (or download it as .zip and unpack)
- The project is set up on Unreal Engine 5.6. - running with an earlier/later version is not advised as things might break.
- All the necessary (and only the necessary) files are included. In order to inspect the code though, generate the solution (RMB on .uproject -> Generate Project Files)
- Running the Editor for the first time (if not ran from the .sln) will prompt you to recompile - click Yes. The compilation should pass without errors.
- In order to test the Multiplayer, switch to Main Menu level, set the amount of Players to 2 and the PlayMode to Standalone Game. Enter Play.
- In one of the Windows that will appear select New Game. In the other (when the first one loads) select Join Session and wait a moment for it to take effect.
- The multiplayer session should be active.
- ⚠️**Important**⚠️ - by default Unreal Engine severely throttles the performance of an unfocused Game window - basically to a 1/2FPS standstill. This will heavily affect a multiplayer session as either Client or Server will be throttled that way, resulting in an enormous lag. This is easily verified by using `stat unit` on both windows and inspecting the frametime of the non-focused window skyrocket. **Keep that in mind when testing the solution** - the lag when running the project that way is not caused by replication issues.

### In the game
- WSAD to move, Spacebar to jump, LMB to shoot, MouseWheel to scroll weapons
- Health is the green number in top-right corner. Ammo is the white number in bottom-right corner.
- Each player starts the match without a weapon.
- Weapon can be picked by interacting with Weapon Pickup on the map. Interaction is automatic and happens when Player gets sufficiently close to a Pickup.
- Picking up a first weapon equips it immediately. Picking up any subsequent Weapons does not - **by design**.
- ⚠️ Equipping a weapon spawns it in Character's hands - but does not switch Animation. Refer to **Known Issues** for more information.
- Picking up a Weapon grants a small amount of ammo. Picking up Ammo pickup grants larger amount of ammo.
- Pistol ammo is yellow. Rifle ammo is blue.
- Players can pick up Ammo for weapons they don't own but won't be able to shoot it unless they pick a correct Weapon.
- Players won't be able to scroll weapons unless they found at least 2.
- Players won't be able to shoot unless they have picked up at least 1 Weapon and have ammo for it.
- Hitting the other Player with a projectile reduces their health.
- If their Health hits 0, they are respawned after a short delay.

## Implementation details
The project is 99% C++ - there is no BP code at all, BPs present in the project are only containers for specialized asset settings (e.g. proper Mesh/Material assignment and Parameter values such as amounts of ammo, time to respawn etc.)

### Weapons
For the Weapons system I chose an unorthodox design, as I wanted to approach it in a different way than directly copying the architecture of TemplateShooterProject, where Weapon logic is implemented in `Weapon` actors. Instead I opted for more data-oriented design using centralized `WeaponComponent` that handles shooting logic as a part of `Character` and `WeaponProp` Actors that only take care of weapon-specific visuals, while all of the control parameters for Weapon types such as Damage, Projectile type etc. are relegated to `WeaponsDatabase`. Depending on the potential direction of the hypothetical larger project, this might be a correct or wrong design choice, mainly depending on types of weapons down the line - whether they'll follow similar projectile-based mechanics or should implement different mechanics for new weapon types. The belefits of my approach:
- New Weapon types can be added 100% by designers without ever needing to interact with C++ code (unless there is a requirement for different shooting mechanics)
- Weapon Props are solely responsible for the visual side
- Weapon types are identified using GameplayTags - this allows for maximum extensibility (e.g. using Enum would require code edits to add a new enum entry)
- Weapon inventory of a player during match is efficiently replicated despite being a centralized collection due to the use of specialized `FFastArraySerializer`

### Pickups
Two types of Pickups exist - Weapons and Ammo. Most of their logic is located in the common `PickupBase` class. They respawn on Timer and are efficiently replicated (deactivation, not destruction). They are interacted with through overlap by design, as this mode of interaction is more in line with multiplayer arena shooters. However, if interaction on button was preferred by designers, it would've been done on Interfaces.

## Known Issues
### Broken Animations
The assets for the Project were delivered with AnimBlueprints that were broken - they required a retarget and after retargetting didn't compile. While technically possible to recreate, I've decided not to dedicate time to it as it didn't seem like a crucial part of the task (as described the aesthetics/visuals of shooting are not the subject of evaluation). Thus, the only Animation used is the one where Character is running without a weapon, even when the Weapon is present (and correctly attached in their hands as a prop).

As a result, I had to fake-out the shooting mechanic a bit. Normally the Projectiles would've been spawned where the Weapon's barrel socket is, but due to animation the barrel would be pointing down. Thus, I spawn Projectiles from a fake-out location connected to the Player Camera (`FakeMuzzle` class for strong typing). 

### Missing SFX/VFX
Shooting feels very dry due to a lack of SFX/VFX feedback. I've prepared Multicast RPCs in the code where those would be fired, but did not implement them due to lack of assets & visuals not being the subject of evaluation.

## Improvement vectors
or "What would you do if you had to dedicate more time"
- Animations fix (see above)
- Aiming recticle (crosshair) & better shooting - Currently due to lack of animation & shooting fakeout, the projectiles are spawned in kind of an awkward way. In a proper projectile-based shooter they should spawn from the barrel but fly towards the aiming recticle. It can be done easily with proper raycasting setup.
- Hitscan weapon mode implementation - it's been prepared as a setting in `FWeaponData` but in the end I decided not to implement it for now, as the task required strictly projectile-based mechanics.

