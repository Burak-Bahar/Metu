import java.util.ArrayList;

public class PlaylistNodePrimaryIndex extends PlaylistNode {
	private ArrayList<Integer> audioIds;
	private ArrayList<PlaylistNode> children;
	
	public PlaylistNodePrimaryIndex(PlaylistNode parent) {
		super(parent);
		audioIds = new ArrayList<Integer>();
		children = new ArrayList<PlaylistNode>();
		this.type = PlaylistNodeType.Internal;
	}
	
	public PlaylistNodePrimaryIndex(PlaylistNode parent, ArrayList<Integer> audioIds, ArrayList<PlaylistNode> children) {
		super(parent);
		this.audioIds = audioIds;
		this.children = children;
		this.type = PlaylistNodeType.Internal;
	}
	
	// GUI Methods - Do not modify
	public ArrayList<PlaylistNode> getAllChildren()
	{
		return this.children;
	}
	
	public PlaylistNode getChildrenAt(Integer index) {return this.children.get(index); }
	
	public Integer audioIdCount()
	{
		return this.audioIds.size();
	}
	public Integer audioIdAtIndex(Integer index) {
		if(index >= this.audioIdCount() || index < 0) {
			return -1;
		}
		else {
			return this.audioIds.get(index);
		}
	}
	
	// Extra functions if needed
	public  Integer get_place(Integer id){
		int position = 0;
		while (position < this.audioIds.size() && this.audioIds.get(position) <= id){
			position += 1;
		}
		return position;
	}
	public void set_child(int index, PlaylistNode child) {
		this.children.add(index, child);
	}
	public void set_value(int index, int value) {
		this.audioIds.add(index, value);
	}
	public  void update_child(int index, PlaylistNode child) {
		this.children.set(index, child);
	}

	public int get_position(Integer id){
		int position = 0;
		for(int i = 0; i < this.audioIdCount(); i++){
			if(this.audioIdAtIndex(i) > id){
				return position;
			}
			else {
				position += 1;
			}
		}
		return position;
	}
}
